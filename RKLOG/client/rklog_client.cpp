
#include "rklog_client.h"

rkLog::rkLog(const char* serverip, unsigned short serverport)
{
    _c = new rpc::client(serverip, serverport);

}

rkLog::~rkLog()
{
    if(_c)
        delete(_c);
}

int rkLog::log(std::string info)
{
    return _c->call("rklog", info).as<int>();
}