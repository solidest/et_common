
#include <string>


//#include "node_modules/nan/nan.h"
#include <nan.h>
#include "rklog_server.h"
#include "readerwriterqueue.h"

using namespace std;

static Nan::Callback * __outcb;
static RklogServer __logsrv;
static uv_async_t __handle;


//send log info to js
void OnLogOut(uv_async_t* handle)
{
    if(__outcb)
    {
        Nan::HandleScope scope;
        auto infos = (ReaderWriterQueue<RkLogInfo> *)handle->data;
        RkLogInfo info;
        while(infos->try_dequeue(info))
        {
            v8::Local<v8::Value> argv[] = {
                Nan::New(info.itype.c_str()).ToLocalChecked(),
                Nan::New(info.info.c_str()).ToLocalChecked()
            };
            Nan::Call(*__outcb, 2, argv);
        }
    }
}

//recv log info from net
void LogOutEvent(ReaderWriterQueue<RkLogInfo>& infos)
{
    __handle.data = &infos;
    uv_async_send(&__handle);
}

//start log server
void StartLogServer(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 3) 
    {
        Nan::ThrowTypeError("Wrong number of arguments");
        return;
    }

    if (!info[0]->IsString() || !info[1]->IsNumber() || !info[2]->IsFunction()) 
    {
        Nan::ThrowTypeError("Wrong arguments");
        return;
    }

    __outcb = new Nan::Callback(info[2].As<v8::Function>());
    uv_async_init(uv_default_loop(), &__handle, OnLogOut);
    __logsrv.HookOut(LogOutEvent);
    
    v8::String::Utf8Value ip(info[0]->ToString());
    rklog_serv_start(__logsrv, *ip, info[1]->Int32Value());
}

//stop log server
void StopLogServer(const Nan::FunctionCallbackInfo<v8::Value>& info)
{
    rklog_serv_stop();
    uv_close((uv_handle_t*)&__handle, nullptr);

    if(__outcb)
    {
        delete __outcb;
        __outcb = nullptr;
    }
}

void Init(v8::Local<v8::Object> exports) 
{
    exports->Set(Nan::New("StartLogServer").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(StartLogServer)->GetFunction());
    exports->Set(Nan::New("StopLogServer").ToLocalChecked(), Nan::New<v8::FunctionTemplate>(StopLogServer)->GetFunction());
}

NODE_MODULE(rklog_server, Init)


// #include <assert.h>
// #include <string>
// #include <iostream>

// #include "uv.h"
// #include "napi.h"
// #include "rklog_server.h"
// #include "readerwriterqueue.h"

// using namespace std;
// static Napi::HandleScope * __scope;
// static Napi::Function * __outcb;
// static RklogServer * __logsrv;
// static uv_async_t __handle;


// void OnLogOut(uv_async_t* handle)
// {
//     if(__outcb)
//     {
//         auto infos = (ReaderWriterQueue<RkLogInfo> *)handle->data;
//         RkLogInfo info;
//         while(infos->try_dequeue(info))
//         {
//             vector<napi_value> args;
//             args.push_back(Napi::String::New(__scope->Env(), info.itype));
//             args.push_back(Napi::String::New(__scope->Env(), info.info));
//             //__outcb->MakeCallback()
//             //__outcb->Call(args);
//             //cout<<info.info<<endl;
//         }
//     }

// }

// void LogOutEvent(ReaderWriterQueue<RkLogInfo>& infos)
// {
//     __handle.data = &infos;
//     uv_async_send(&__handle);
// }

// Napi::Value StartLogServer(const Napi::CallbackInfo& info)
// {
//     Napi::Env env = info.Env();

//     if (info.Length() < 1)
//     {
//         Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
//         return env.Undefined();
//     }

//     if (!info[0].IsFunction())
//     {
//         Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
//         return env.Undefined();
//     }

//     __scope = new Napi::HandleScope(env);
//     __outcb = new Napi::Function(info[0].As<Napi::Function>());
//     __logsrv = new RklogServer(LogOutEvent);

//     uv_loop_s * loop = nullptr;
//     napi_get_uv_event_loop(env, &loop);
//     assert(loop != nullptr);
//     uv_async_init(loop, &__handle, OnLogOut);

//     rklog_serv_start(*__logsrv, "127.0.0.1", 5000);
//     return env.Undefined();
// }

// Napi::Value StopLogServer(const Napi::CallbackInfo& info)
// {
//     Napi::Env env = info.Env();

//     rklog_serv_stop();
//     uv_close((uv_handle_t*)&__handle, nullptr);

//     if(__logsrv)
//     {
//          delete __logsrv;
//          __logsrv = nullptr;
//     }

//     if(__scope)
//     {
//         delete __scope;
//         __outcb = nullptr;      
//     }

//     if(__outcb)
//     {
//         delete __outcb;
//         __outcb = nullptr;
//     }

//     return env.Undefined();
// }


// Napi::Object Init(Napi::Env env, Napi::Object exports) 
// {
//     exports.Set(Napi::String::New(env, "StartLogServer"), Napi::Function::New(env, StartLogServer));
//     exports.Set(Napi::String::New(env, "StopLogServer"), Napi::Function::New(env, StopLogServer));

//     return exports;
// }

// NODE_API_MODULE(addon, Init)