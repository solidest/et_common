/*
** rkdb_server.h for rkdb in /home/byx/et/et_common/RKDB/server
**
** Made by header_template
** Login   <>
**
** Started on  undefined Dec 9 6:49:34 PM 2018 header_template
** Last update Tue Dec 10 11:26:11 PM 2018 header_template
*/

#ifndef RKDB_SERVER_H_
# define RKDB_SERVER_H_
#include <chrono>
#include <atomic>
#include <string>
#include <vector>

#include <unistd.h>
#include "rpc/server.h"
#include "rocksdb/db.h"

#define TIME_STARTPOINT 1514736000000    //2018-01-01 00:00:00.000

#define DB_FILE "~/db/rocks_data" 
#define STR_CREATE_TIME "CreateTime"
#define STR_UPDATE_TIME "UpdateTime"
#define STR_INFO_VALUE "ProjectInfo"

#define COLUMN_PROJECT_INFO 1
#define COLUMN_PROJECT      2
#define COLUMN_CASE         3
#define COLUMN_CASE_RUN     4
#define COLUMN_RUN_IO       5
#define COLUMN_RUN_ASSERT   6
#define COLUMN_RUN_INFO     7

using namespace std::chrono;
using namespace std;
using namespace rocksdb;

typedef time_point<system_clock, milliseconds> microClock_type1; 
typedef time_point<steady_clock, milliseconds> microClock_type2; 
typedef std::atomic<long long> AtomicInt64;
typedef std::atomic<int> AtomicInt;
typedef std::atomic<bool> AtomicBool;


class RkdbServer
{
public:
    RkdbServer(int kyid);
    ~RkdbServer();

    string GetProjectInfoList();
    long long NewProjectInfo(string & value);
    void UpdateProjectInfo(long long & pid, string & value);
    void SaveProject(long long & pid, string & vlaue);
    string OpenProject(long long & pid);
    void DeleProject(long long & pid);

    // long long StartRunCase(long long & pid, long long & caseid);
    // void SaveIO(long long & rid, string & value);
    // void SaveAssert(long long & rid, string & value);
    // void SaveInfo(long long & rid, string & value);
    // void StopRunCase();

private:
    inline bool CheckIsRunning();
    inline long long GetNow();
    inline long long GetNewId();
    inline long long tilNextMillis(long long lastTimestamp);
    inline bool existsFile (const char* fileName) 
    {
        return (access(fileName, F_OK) != -1);
    }

    DB* _db;
    std::vector<ColumnFamilyHandle*> _col_handles;

    AtomicBool _is_runcase { false };
    AtomicInt64 _runcase_id { 0 };
    AtomicInt64 _last_timestamp { 0 };
    AtomicInt _sequence { 0 };
    long long _time_diff;
    int _kyid;
};





#endif /* !RKDB_SERVER_H_ */
