
#include <iostream>
#include "rklog_client.h"
#include "rpc/rpc_error.h"

RklogClient::RklogClient(const char* clientname, const char* serverip, unsigned short serverport)
{
    _name = clientname;
    _c = new rpc::client(serverip, serverport);
    _c->set_timeout(1000);
}

RklogClient::~RklogClient()
{
    if(_c)
    {
        delete(_c);
        _c = nullptr;      
    }
}

inline void RklogClient::log(const std::string& itype, const std::string& info)
{
    try 
    {
        _c->call(itype, _name, info);
    } 
    catch (rpc::timeout &t)
    {
        std::cerr << t.what() << std::endl;
        std::cout << itype << "> " << info << std::endl;
    }
}

void RklogClient::LogInfo(const std::string& info)
{
    log("LogInfo", info);
}

void RklogClient::LogError(const std::string& info)
{
    log("LogError", info);
}
void RklogClient::LogWarning(const std::string& info)
{
    log("LogWarning", info);
}
void RklogClient::LogDebug(const std::string& info)
{
    log("LogDebug", info);
}