
#include <iostream>

#include "rpc/server.h"
#include "rklog_server.h"


static rpc::server* psrv;

//start log server
int rklog_serv_start(RklogServer& log, const char* serverip, unsigned short serverport)
{
    psrv = new rpc::server(serverip, serverport);
    
    psrv->bind("LogInfo", [&log](string const & info) { log.LogInfo(info); });
    psrv->bind("LogError", [&log](string const & info) { log.LogError(info); });
    psrv->bind("LogWarning", [&log](string const & info) { log.LogWarning(info); });
    psrv->bind("LogDebug", [&log](string const & info) { log.LogDebug(info); });

    psrv->async_run();

    return 0;
}

//stop log server
int rklog_serv_stop()
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

RklogServer::RklogServer(void (*outhook)(ReaderWriterQueue<RkLogInfo>&)):_infos(100)
{
    _OutHook = outhook;
}

RklogServer::~RklogServer()
{
    if(_OutHook)
    {
        _OutHook(_infos);        
    }
}

//log out
void RklogServer::Log(const string & itype, const string & info)
{
    RkLogInfo loginfo = 
    {
        .itype = itype,
        .info = info,
    };

    if(_OutHook)
    {
        _infos.try_enqueue(loginfo);
        _OutHook(_infos);        
    }
    else
        std::cout<<loginfo.itype<<">"<<" "<<loginfo.info<<std::endl;
}


void RklogServer::LogInfo(const string& info)
{
    Log(RKLOG_INFO_KEY, info);
}

void RklogServer::LogError(const string& info)
{
    Log(RKLOG_ERROR_KEY, info);
}

void RklogServer::LogWarning(const string& info)
{
    Log(RKLOG_WARNING_KEY, info);
}

void RklogServer::LogDebug(const string& info)
{
    Log(RKLOG_DEBUG_KEY, info);
}


