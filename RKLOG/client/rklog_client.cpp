
#include <iostream>
#include "rklog_client.h"
#include "rpc/rpc_error.h"

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
    try {
        // default timeout is 5000 milliseconds
        const uint64_t short_timeout = 1000;
        _c->set_timeout(short_timeout);
        _c->call("rklog", info).as<int>();
    } catch (rpc::timeout &t) {
        // will display a message like
        // rpc::timeout: Timeout of 50ms while calling RPC function 'sleep'
        std::cout << t.what() << std::endl;
    }

    return 0;
}