/*
** EPITECH PROJECT, 2018
** rklog
** File description:
** 
*/

#ifndef RKLOG_H_
#define RKLOG_H_

#define RKLOG_INFO_KEY "info"
#define RKLOG_ERROR_KEY "error"
#define RKLOG_WARNING_KEY "warning"
#define RKLOG_DEBUG_KEY "debug"

#include <string.h>
#include "readerwriterqueue.h"

using namespace std;
using namespace moodycamel;

typedef struct RkLogInfo
{
    string itype;
    string info;
} RkLogInfo;

class RklogServer
{
public:
    RklogServer();
    ~RklogServer();
    void HookOut(void (*outhook)(ReaderWriterQueue<RkLogInfo>&));
    void LogInfo(const string& info);
    void LogError(const string& info);
    void LogWarning(const string& info);
    void LogDebug(const string& info);

private:
    void (*_OutHook)(ReaderWriterQueue<RkLogInfo>&);
    void Log(const string & itype, const string & info);    
    ReaderWriterQueue<RkLogInfo> _infos;
};

int rklog_serv_start(RklogServer& log, const char* serverip, unsigned short serverport);
int rklog_serv_stop();

#endif /* !RKLOG_H_ */
