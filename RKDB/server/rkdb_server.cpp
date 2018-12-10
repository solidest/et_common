

#include "rkdb_server.h"
#include "rpc/this_handler.h"

//initial db server
RkdbServer::RkdbServer(int kyid)
{
    _kyid = kyid;
    microClock_type1 tp1 = time_point_cast<milliseconds>(system_clock::now());
    microClock_type2 tp2 = time_point_cast<milliseconds>(steady_clock::now()); 
    _time_diff = tp1.time_since_epoch().count() - tp2.time_since_epoch().count() - TIME_STARTPOINT;   //2018-01-01 00:00:00.000
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
    if(!_is_runcase)
        return false;
    auto err_obj = std::make_tuple(1, "One Case Is Running");
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

//new project file
long long RkdbServer::NewProject(string & name, string & notes)
{
    if(CheckIsRunning()) return 0;
    ProjectInfo pinfo;
    pinfo.create_time = pinfo.update_time = GetNow() + TIME_STARTPOINT;
    memcpy(&pinfo.pj_name, name.c_str, min(sizeof(pinfo.pj_name), name.size()));
    memcpy(&pinfo.pj_notes, notes.c_str, min(sizeof(pinfo.pj_notes), name.size()));
    
}

//save project file
void RkdbServer::SaveProject(long long & pid, string & vlaue)
{


}

//start run a case
long long RkdbServer::StartRunCase(long long & pid, long long & caseid)
{
    if(!CheckIsRunning()) return 0;

    _is_runcase = true;

    _runcase_id =  GetNewId();
   

    return _runcase_id;
}

//stop run a case
void RkdbServer::StopRunCase()
{

    _is_runcase = false;
}


//id is full in one millis
long long RkdbServer::tilNextMillis(long long lastTimestamp)
{
    long long timestamp = GetNow();
    while (timestamp == lastTimestamp) {
        timestamp = GetNow();
    }
    return timestamp;
}

