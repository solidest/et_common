
#include <iostream>

#include "rpc/server.h"
#include "rklog_server.h"

//start log server
// int rklog_serv_start(const char* serverip, unsigned short serverport)
// {
//     rpc::server srv(serverip, serverport); // listen on TCP port
    
//     //LogContext g_log_context;
//     //if(logfile != NULL) log_set_filename(logfile);

//     //log_init();
   
//     srv.bind("rklog", &rklog);
//     constexpr size_t thread_count = 3;

//     srv.async_run(); // non-blocking call, handlers execute on one of the workers

//     std::cin.ignore();
//     return 0;
// }

// void rklog(std::string itype, std::string info)
// {
//     RkLogInfo * plogi = new RkLogInfo;
//     plogi->info = info;
//     plogi->itype = itype;
//     __infos.try_enqueue(plogi);
    
// } 

RklogServer::RklogServer():_infos(100)
{

}

void RklogServer::Log(const RkLogInfo & loginfo)
{
    _infos.try_enqueue(loginfo);
}


