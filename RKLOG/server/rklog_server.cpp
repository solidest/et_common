
#include <iostream>
#include <string.h>
#include "rpc/server.h"
#include "rklog_server.h"

int rklog(std::string info)
{
    std::cout<<"RKLOG>  "<<info<<std::endl;
} 


//start log server
int rklog_serv_start(const char* serverip, unsigned short serverport, const char* logfile)
{
    rpc::server srv(serverip, serverport); // listen on TCP port

    srv.bind("rklog", &rklog);

    constexpr size_t thread_count = 3;

    srv.async_run(thread_count); // non-blocking call, handlers execute on one of the workers

    std::cin.ignore();
    return 0;
}

