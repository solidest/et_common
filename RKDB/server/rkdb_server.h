/*
** rkdb_server.h for rkdb in /home/byx/et/et_common/RKDB/server
**
** Made by header_template
** Login   <>
**
** Started on  undefined Dec 9 6:49:34 PM 2018 header_template
** Last update Fri Dec 13 4:31:36 PM 2018 header_template
*/

#ifndef RKDB_SERVER_H_
# define RKDB_SERVER_H_
#include <chrono>
#include <atomic>
#include <string>
#include <vector>
#include <mutex>
#include <unistd.h>

#include "rocksdb/db.h"

#define TIME_STARTPOINT 1514736000000    //2018-01-01 00:00:00.000

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


#define ERR_CODE_ISRUNCASE  1

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
    RkdbServer(int kyid, const char* dbfile);
    ~RkdbServer();

    string GetProjectInfoList();
    long long NewProjectInfo(string const & value);
    void SetProjectInfo(long long pid, string const & value);
    void SetProject(long long pid, string const & vlaue);
    string GetProject(long long pid);
    void DelProject(long long pid);

    // long long StartRunCase(long long & pid, long long & caseid);
    // void SaveIO(long long & rid, string & value);
    // void SaveAssert(long long & rid, string & value);
    // void SaveInfo(long long & rid, string & value);
    // void StopRunCase();

private:
    inline bool isRunningCase();
    inline long long getNow();
    inline long long getNewId();
    inline long long tillNextMillis(long long lastTimestamp);
    inline bool existsFile (const char* fileName) 
    {
        return (access(fileName, F_OK) != -1);
    }

    DB* _db;
    std::vector<ColumnFamilyHandle*> _col_handles;
    AtomicBool _is_runcase { false };
    long long _time_diff;
    int _kyid;
    std::mutex _mtx;
};


int rkdb_serv_start(RkdbServer& db, const char* serverip, unsigned short serverport);
int rkdb_serv_stop();


#endif /* !RKDB_SERVER_H_ */
