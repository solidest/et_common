/*
** rkdb_server.h for rkdb in /home/byx/et/et_common/RKDB/server
**
** Made by header_template
** Login   <>
**
** Started on  undefined Dec 9 6:49:34 PM 2018 header_template
** Last update Mon Dec 9 6:50:59 PM 2018 header_template
*/

#ifndef RKDB_SERVER_H_
# define RKDB_SERVER_H_
#include <chrono>
#include <atomic>
#include <string>
#include<vector>
#include "rpc/server.h"

#define TIME_STARTPOINT 1514736000000

using namespace std::chrono;
using namespace std;

typedef time_point<system_clock, milliseconds> microClock_type1; 
typedef time_point<steady_clock, milliseconds> microClock_type2; 
typedef std::atomic<long long> AtomicInt64;
typedef std::atomic<int> AtomicInt;
typedef std::atomic<bool> AtomicBool;

struct ProjectInfo
{
    char pj_name[40];
    long long create_time;
    long long update_time;
    char pj_notes[200];
};


class RkdbServer
{
public:
    RkdbServer(int kyid);

    string GetProjectList();
    long long NewProject(string & name, string & notes);
    void RenameProject(long long & pid, string & name, string & notes);

    void SaveProject(long long & pid, string & vlaue);
    string OpenProject(long long & pid);
    void DeleProject(long long & pid);

    long long StartRunCase(long long & pid, long long & caseid);
    void SaveIO(long long & rid, string & value);
    void SaveAssert(long long & rid, string & value);
    void SaveInfo(long long & rid, string & value);
    void StopRunCase();

private:
    bool CheckIsRunning();
    long long GetNow();
    long long GetNewId();
    long long tilNextMillis(long long lastTimestamp);

    AtomicBool _is_runcase = false;
    AtomicInt64 _runcase_id { 0 };
    AtomicInt64 _last_timestamp { 0 };
    AtomicInt _sequence { 0 };
    long long _time_diff;
    int _kyid;
};





#endif /* !RKDB_SERVER_H_ */
