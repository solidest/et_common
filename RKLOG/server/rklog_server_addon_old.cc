


#include <assert.h>
#include <string>
#include <iostream>

#include "uv.h"
#include "napi.h"
#include "rklog_server.h"
#include "readerwriterqueue.h"

using namespace std;
static Napi::HandleScope * __scope;
static Napi::Function * __outcb;
static RklogServer * __logsrv;
static uv_async_t __handle;


void OnLogOut(uv_async_t* handle)
{
    if(__outcb)
    {
        auto infos = (ReaderWriterQueue<RkLogInfo> *)handle->data;
        RkLogInfo info;
        while(infos->try_dequeue(info))
        {
            vector<napi_value> args;
            args.push_back(Napi::String::New(__scope->Env(), info.itype));
            args.push_back(Napi::String::New(__scope->Env(), info.info));
            __outcb->Call(args);
            //cout<<info.info<<endl;
        }
    }

}

void LogOutEvent(ReaderWriterQueue<RkLogInfo>& infos)
{
    __handle.data = &infos;
    uv_async_send(&__handle);
}

Napi::Value StartLogServer(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].IsFunction())
    {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    __scope = new Napi::HandleScope(env);
    __outcb = new Napi::Function(info[0].As<Napi::Function>());
    __logsrv = new RklogServer(LogOutEvent);

    uv_loop_s * loop = nullptr;
    napi_get_uv_event_loop(env, &loop);
    assert(loop != nullptr);
    uv_async_init(loop, &__handle, OnLogOut);

    rklog_serv_start(*__logsrv, "127.0.0.1", 5000);
    return env.Undefined();
}

Napi::Value StopLogServer(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    rklog_serv_stop();
    uv_close((uv_handle_t*)&__handle, nullptr);

    if(__logsrv)
    {
         delete __logsrv;
         __logsrv = nullptr;
    }

    if(__scope)
    {
        delete __scope;
        __outcb = nullptr;      
    }

    if(__outcb)
    {
        delete __outcb;
        __outcb = nullptr;
    }

    return env.Undefined();
}


Napi::Object Init(Napi::Env env, Napi::Object exports) 
{
    exports.Set(Napi::String::New(env, "StartLogServer"), Napi::Function::New(env, StartLogServer));
    exports.Set(Napi::String::New(env, "StopLogServer"), Napi::Function::New(env, StopLogServer));

    return exports;
}

NODE_API_MODULE(addon, Init)