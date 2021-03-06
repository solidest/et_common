
#include <iostream>
#include "rkdb_server.h"

#include "rpc/this_handler.h"
#include "rpc/server.h"
#include "rpc/this_server.h"

#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "rocksdb/utilities/transaction.h"
#include "rocksdb/utilities/transaction_db.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


using namespace rapidjson;

static rpc::server* psrv;

//db server start
int rkdb_serv_start(RkdbServer& db, const char* serverip, unsigned short serverport)
{
    //rpc::server srv(serverport); // listen on TCP port
    psrv = new rpc::server(serverip, serverport);
    
    psrv->bind("GetProjectInfoList", [&db]()->string { return db.GetProjectInfoList(); });
    psrv->bind("NewProjectInfo", [&db](string const & value)->long long { return db.NewProjectInfo(value); });
    psrv->bind("SetProjectInfo", [&db](long long pid, string const & value){ db.SetProjectInfo(pid, value); });
    psrv->bind("SetProject", [&db](long long pid, string const & value){ db.SetProject(pid, value); });
    psrv->bind("GetProject", [&db](long long pid)->string { return db.GetProject(pid); });
    psrv->bind("DelProject", [&db](long long pid){ db.DelProject(pid); });

    psrv->async_run(2);

    return 0;
}

int rkdb_serv_stop()
{
    if(psrv)
    {
        psrv->close_sessions();
        psrv->stop();
        delete psrv;
        psrv = nullptr;
    }

    return 0;
}


//initial db server
RkdbServer::RkdbServer(int kyid, const char* dbfile)
{
    _kyid = kyid;
    microClock_type1 tp1 = time_point_cast<milliseconds>(system_clock::now());
    microClock_type2 tp2 = time_point_cast<milliseconds>(steady_clock::now()); 
    _time_diff = tp1.time_since_epoch().count() - tp2.time_since_epoch().count() - TIME_STARTPOINT; 

    
    if(!existsFile(dbfile))
    {
        Options options;
        options.create_if_missing = true;
        Status s = DB::Open(options, dbfile, &_db);
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

        Status s = DB::Open(DBOptions(), dbfile, column_families, &_col_handles, &_db);
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
    if(isRunningCase()) return 0;
    Document doc(kArrayType);
    Document::AllocatorType& allocator = doc.GetAllocator();

    ReadOptions ro;
    
    Iterator* iterator = _db->NewIterator(ReadOptions(), _col_handles[COLUMN_PROJECT_INFO]);
    for (iterator->SeekToFirst(); iterator->Valid(); iterator->Next())
    {
        Value v(kObjectType);
        Value key(kNumberType);
        Value val(kStringType);
        key.SetInt64(stoll(iterator->key().ToString()));
        val.SetString(iterator->value().data(), iterator->value().size());
        v.AddMember("key", key, allocator);
        v.AddMember("value", val, allocator);
        doc.PushBack(v, allocator);
    }
    delete iterator;
    
    StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    string ret;
    ret.assign(buffer.GetString(), buffer.GetSize());
    return ret;
}


//new project file
long long RkdbServer::NewProjectInfo(string const & value)
{
    if(isRunningCase()) return 0;

    auto ti = getNow() + TIME_STARTPOINT;
    Value vi(kNumberType);
    vi.SetInt64(ti);
    Value vi2(kNumberType);
    vi2.SetInt64(ti);
    Value vs(kStringType);
    vs.SetString(value.c_str(), value.size());
    Document doc(kObjectType);
    doc.AddMember(STR_CREATE_TIME, vi, doc.GetAllocator());
    doc.AddMember(STR_UPDATE_TIME, vi2, doc.GetAllocator());
    doc.AddMember(STR_INFO_VALUE, vs, doc.GetAllocator());

    auto id = getNewId();
    char sid[20] = { 0 };
    sprintf(sid, "%019lld", id);
    Slice slkey = sid;
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    Slice slvalue(buffer.GetString(), buffer.GetSize());
    string empty("");
    Slice slv = empty;

    WriteBatch batch;
    batch.Put(_col_handles[COLUMN_PROJECT_INFO], slkey, slvalue);
    batch.Put(_col_handles[COLUMN_PROJECT], slkey, slv);
    Status s = _db->Write(WriteOptions(), &batch);
    assert(s.ok());
    return id;
}

//update project info
void RkdbServer::SetProjectInfo(long long pid, string const & value)
{
    if(isRunningCase()) return;

    string v;
    long long ti = getNow() + TIME_STARTPOINT;
    char sid[20] = { 0 };
    sprintf(sid, "%019lld", pid);
    Slice slkey = sid;

    Status s = _db->Get(ReadOptions(), _col_handles[COLUMN_PROJECT_INFO], slkey, &v);
    assert(s.ok());
    Document doc;
    doc.Parse(v.c_str());
    doc[STR_UPDATE_TIME].SetInt64(ti);
    doc[STR_INFO_VALUE].SetString(value.c_str(), value.size());
    
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    Slice slvalue(buffer.GetString(), buffer.GetSize());
    s = _db->Put(WriteOptions(), _col_handles[COLUMN_PROJECT_INFO], slkey, slvalue);
    assert(s.ok());
}

//delete one project
void RkdbServer::DelProject(long long pid)
{
    if(isRunningCase()) return; 
    WriteBatch batch;
    char sid[20] = { 0 };
    sprintf(sid, "%019lld", pid);
    Slice slkey = sid;
    batch.Delete(_col_handles[COLUMN_PROJECT_INFO], slkey);
    batch.Delete(_col_handles[COLUMN_PROJECT], slkey);
    Status s = _db->Write(WriteOptions(), &batch);
    assert(s.ok());
}

//save project content
void RkdbServer::SetProject(long long pid, string const & value)
{
    if(isRunningCase()) return;
    char sid[20] = { 0 };
    sprintf(sid, "%019lld", pid);
    Slice slkey = sid;

    //record update time
    string v;
    auto ti = getNow() + TIME_STARTPOINT;
    WriteBatch batch;

    Status s = _db->Get(ReadOptions(), _col_handles[COLUMN_PROJECT_INFO], slkey, &v);
    assert(s.ok());
    Document doc;
    doc.Parse(v.c_str());
    doc[STR_UPDATE_TIME].SetInt64(ti);
    StringBuffer buffer;
    Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    Slice slvalue(buffer.GetString(), buffer.GetSize());

    //save project info & content
    batch.Put(_col_handles[COLUMN_PROJECT_INFO], slkey, slvalue);
    slvalue = value;
    batch.Put(_col_handles[COLUMN_PROJECT], slkey, slvalue);

    s = _db->Write(WriteOptions(), &batch);
    assert(s.ok());
}

//get project content
string RkdbServer::GetProject(long long pid)
{
    string ret;
    char sid[20] = { 0 };
    sprintf(sid, "%019lld", pid);
    Slice slkey = sid;
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
inline long long RkdbServer::tillNextMillis(long long lastTimestamp)
{
    long long timestamp = getNow();
    while (timestamp == lastTimestamp) {
        timestamp = getNow();
    }
    return timestamp;
}


//get now time tick
inline long long RkdbServer::getNow()
{
    microClock_type2 tp = time_point_cast<milliseconds>(steady_clock::now()); 
    long long ret = tp.time_since_epoch().count() + _time_diff;
    return ret;
}

//check is server idle
inline bool RkdbServer::isRunningCase()
{
    if(!_is_runcase) return false;
    auto err_obj = std::make_tuple(ERR_CODE_ISRUNCASE, "One Case Is Running");
    rpc::this_handler().respond_error(err_obj);
    return true;
}


//get a new id
inline long long RkdbServer::getNewId()
{
    static long long __last_timestamp = 0;
    static int __sequence = 0;
    long long timestamp = getNow();

    _mtx.lock();

    if (__last_timestamp == timestamp) 
    {
        __sequence = (__sequence+1) & 4095;
        if (0 == __sequence) timestamp = tillNextMillis(__last_timestamp);
    }
    else
    {
        __sequence = 0;
    }

    long long ret = (timestamp << 22) | ((_kyid & 0x3FF) << 12) | (__sequence & 0xFFF);

    __last_timestamp = timestamp;

    _mtx.unlock();

    return ret;
}

