
#include "rkdb_server.h"
#include "rpc/this_handler.h"

#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


using namespace rapidjson;

//initial db server
RkdbServer::RkdbServer(int kyid)
{
    _kyid = kyid;
    microClock_type1 tp1 = time_point_cast<milliseconds>(system_clock::now());
    microClock_type2 tp2 = time_point_cast<milliseconds>(steady_clock::now()); 
    _time_diff = tp1.time_since_epoch().count() - tp2.time_since_epoch().count() - TIME_STARTPOINT; 

    string kDBPath = DB_FILE;
    if(!existsFile(DB_FILE))
    {
        Options options;
        options.create_if_missing = true;
        Status s = DB::Open(options, kDBPath, &_db);
        assert(s.ok());

        // create column family
        _col_handles.push_back(_db->DefaultColumnFamily());
        ColumnFamilyHandle* cf;

        s = _db->CreateColumnFamily(ColumnFamilyOptions(), "project_info", &cf);
        assert(s.ok());
        _col_handles.push_back(cf);
        s = _db->CreateColumnFamily(ColumnFamilyOptions(), "project", &cf);
        assert(s.ok());
        _col_handles.push_back(cf);
        s = _db->CreateColumnFamily(ColumnFamilyOptions(), "case", &cf);
        assert(s.ok());
        _col_handles.push_back(cf);
        s = _db->CreateColumnFamily(ColumnFamilyOptions(), "case_run", &cf);
        assert(s.ok());
        _col_handles.push_back(cf);
        s = _db->CreateColumnFamily(ColumnFamilyOptions(), "run_io", &cf);
        assert(s.ok());
        _col_handles.push_back(cf);
        s = _db->CreateColumnFamily(ColumnFamilyOptions(), "run_assert", &cf);
        assert(s.ok());
        _col_handles.push_back(cf);
        s = _db->CreateColumnFamily(ColumnFamilyOptions(), "run_info", &cf);
        assert(s.ok());
        _col_handles.push_back(cf);
    }
    else
    {
        std::vector<ColumnFamilyDescriptor> column_families;
        // have to open default column family
        column_families.push_back(ColumnFamilyDescriptor(kDefaultColumnFamilyName, ColumnFamilyOptions()));
        // open the new one, too
        column_families.push_back(ColumnFamilyDescriptor("project_info", ColumnFamilyOptions()));
        column_families.push_back(ColumnFamilyDescriptor("project", ColumnFamilyOptions()));
        column_families.push_back(ColumnFamilyDescriptor("case", ColumnFamilyOptions()));
        column_families.push_back(ColumnFamilyDescriptor("case_run", ColumnFamilyOptions()));
        column_families.push_back(ColumnFamilyDescriptor("run_io", ColumnFamilyOptions()));
        column_families.push_back(ColumnFamilyDescriptor("run_assert", ColumnFamilyOptions()));
        column_families.push_back(ColumnFamilyDescriptor("run_info", ColumnFamilyOptions()));

        Status s = DB::Open(DBOptions(), kDBPath, column_families, &_col_handles, &_db);
        assert(s.ok());
    }

}

//destroy db server
RkdbServer::~RkdbServer()
{
    for(int i=1; i<_col_handles.size(); i++)
    {
        delete _col_handles[i];
    }

    delete _db;  
}

//get all project info
string RkdbServer::GetProjectInfoList()
{
    if(CheckIsRunning()) return 0;
    Document doc(kArrayType);
    Document::AllocatorType& allocator = doc.GetAllocator();

    Iterator* iterator = _db->NewIterator(ReadOptions(), _col_handles[COLUMN_PROJECT_INFO]);

    int ii = 1;
    for (iterator->SeekToFirst(); iterator->Valid(); iterator->Next())
    {
        Value v(kObjectType);
        Value vv(kStringType);
        Value key(kNumberType);
        key.SetInt64(*(long long *)iterator->key().data());
        vv.SetString(iterator->value().data(), allocator);
        v.AddMember("key", key, allocator);
        v.AddMember("value", vv, allocator);
        doc.PushBack(v, allocator);
    }
    
    StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    return buffer.GetString();
}


//new project file
long long RkdbServer::NewProjectInfo(string & value)
{
    if(CheckIsRunning()) return 0;

    auto ti = GetNow() + TIME_STARTPOINT;
    Value vi(kNumberType);
    vi.SetInt64(ti);
    Value vs(kStringType);
    vs.SetString(value.c_str(), value.size());
    Document doc(kObjectType);
    doc.AddMember(STR_CREATE_TIME, vi, doc.GetAllocator());
    doc.AddMember(STR_UPDATE_TIME, vi, doc.GetAllocator());
    doc.AddMember(STR_INFO_VALUE, vs, doc.GetAllocator());

    auto id = GetNewId();
    Slice slkey((char *)&id, 8);
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    Slice slvalue = buffer.GetString();
    Status s = _db->Put(WriteOptions(), _col_handles[COLUMN_PROJECT_INFO], slkey, slvalue);
    assert(s.ok());
    return id;
}

//update project info
void RkdbServer::UpdateProjectInfo(long long & pid, string & value)
{
    if(CheckIsRunning()) return;

    string v;
    long long ti = GetNow() + TIME_STARTPOINT;
    rocksdb::Slice slkey((char *)&pid, 8);

    Status s = _db->Get(ReadOptions(), _col_handles[COLUMN_PROJECT_INFO], slkey, &v);
    assert(s.ok());
    Document doc;
    doc.Parse(v.c_str());
    doc[STR_UPDATE_TIME].SetInt64(ti);
    doc[STR_INFO_VALUE].SetString(value.c_str(), value.size());
    
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    Slice slvalue = buffer.GetString();
    s = _db->Put(WriteOptions(), _col_handles[COLUMN_PROJECT_INFO], slkey, slvalue);
    assert(s.ok());
}

//delete one project
void RkdbServer::DeleProject(long long & pid)
{
    if(CheckIsRunning()) return; 
    WriteBatch batch;
    rocksdb::Slice slkey((char *)&pid, 8);
    batch.Delete(_col_handles[COLUMN_PROJECT_INFO], slkey);
    batch.Delete(_col_handles[COLUMN_PROJECT], slkey);
    Status s = _db->Write(WriteOptions(), &batch);
    assert(s.ok());
}

//save project content
void RkdbServer::SaveProject(long long & pid, string & value)
{
    if(CheckIsRunning()) return;
    Slice slkey((const char *)&pid, 8);

    //record update time
    string v;
    auto ti = GetNow() + TIME_STARTPOINT;
    WriteBatch batch;

    Status s = _db->Get(ReadOptions(), _col_handles[COLUMN_PROJECT_INFO], slkey, &v);
    assert(s.ok());
    Document doc;
    doc.Parse(v.c_str());
    doc[STR_UPDATE_TIME].SetInt64(ti);
    StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    Slice slvalue = buffer.GetString();

    //save project info & content
    batch.Put(_col_handles[COLUMN_PROJECT_INFO], slkey, slvalue);
    slvalue = value;
    batch.Put(_col_handles[COLUMN_PROJECT], slkey, slvalue);

    s = _db->Write(WriteOptions(), &batch);
    assert(s.ok());
}

//get project content
string RkdbServer::OpenProject(long long & pid)
{
    string ret;
    rocksdb::Slice slkey((char *)&pid, 8);
    Status s = _db->Get(ReadOptions(), _col_handles[COLUMN_PROJECT], slkey, &ret);
    assert(s.ok());
    return ret;
}

// //start run a case
// long long RkdbServer::StartRunCase(long long & pid, long long & caseid)
// {
//     if(!CheckIsRunning()) return 0;

//     _is_runcase = true;

//     _runcase_id =  GetNewId();
   

//     return _runcase_id;
// }

// //stop run a case
// void RkdbServer::StopRunCase()
// {

//     _is_runcase = false;
// }


//id is full in one millis
inline long long RkdbServer::tilNextMillis(long long lastTimestamp)
{
    long long timestamp = GetNow();
    while (timestamp == lastTimestamp) {
        timestamp = GetNow();
    }
    return timestamp;
}


//get now time tick
inline long long RkdbServer::GetNow()
{
    microClock_type2 tp = time_point_cast<milliseconds>(steady_clock::now()); 
    long long ret = tp.time_since_epoch().count() + _time_diff;
    return ret;
}

//check is server idle
inline bool RkdbServer::CheckIsRunning()
{
    if(!_is_runcase) return false;
    auto err_obj = std::make_tuple(ERR_CODE_ISRUNCASE, "One Case Is Running");
    rpc::this_handler().respond_error(err_obj);
    return true;
}


//get a new id
inline long long RkdbServer::GetNewId()
{
    AtomicInt64 timestamp{ 0 };
    timestamp = GetNow();

    if (_last_timestamp == timestamp) 
    {
        _sequence = (_sequence + 1) & 4095;
        if (0 == _sequence) timestamp = tilNextMillis(_last_timestamp);
    }
    else 
    {
        _sequence = 0;
    }

    _last_timestamp = timestamp.load();

    return (timestamp << 22) | (_kyid << 12) | _sequence;
}

