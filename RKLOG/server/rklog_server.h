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

private:
    void Log(const RkLogInfo & loginfo);    
    ReaderWriterQueue<RkLogInfo> _infos;
};

int rklog_serv_start(RklogServer& logsrv, const char* serverip, unsigned short serverport);


#endif /* !RKLOG_H_ */
