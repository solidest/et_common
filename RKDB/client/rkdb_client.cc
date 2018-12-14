

#include <assert.h>
#include <string>

#include "napi.h"
#include "rpc/client.h"
#include "rpc/rpc_error.h"


using namespace std;

static rpc::client * __c;

Napi::Value OpenRkdb(const Napi::CallbackInfo& info) 
{
    Napi::Env env = info.Env();
    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    if (!info[0].IsString() || !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    auto ip = info[0].As<Napi::String>().Utf8Value();
    auto port = info[1].As<Napi::Number>().Int32Value();
    __c = new rpc::client(ip, port);

    return env.Undefined();
}

Napi::Value CloseRkdb(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    if(__c)
    {
        delete __c;
    }
    return env.Undefined();
}

// string GetProjectInfoList();
Napi::Value GetProjectInfoList(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    try
    {
        auto pl = __c->call("GetProjectInfoList").as<string>();
        Napi::String ret = Napi::String::New(env, pl);
        return ret;
    }
    catch (rpc::timeout &t) 
    {
        Napi::TypeError::New(env, "Timeout when call rkdbserver").ThrowAsJavaScriptException();
    }
    catch (rpc::rpc_error &t)
    {
        using err_t = std::tuple<int, std::string>;
        auto err = t.get_error().as<err_t>();
        Napi::TypeError::New(env, std::get<1>(err)).ThrowAsJavaScriptException();
    }
    return env.Null();
}

// long long NewProjectInfo(string const & value);
Napi::Value NewProjectInfo(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    try
    {
        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsString())
        {
            Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto pjinfo = info[0].As<Napi::String>().Utf8Value();
        auto id = __c->call("NewProjectInfo", pjinfo).as<long long>();
        Napi::Number ret = Napi::Number::New(env, id);
        return ret;
    }
    catch (rpc::timeout &t) 
    {
        Napi::TypeError::New(env, "Timeout when call rkdbserver").ThrowAsJavaScriptException();
    }
    catch (rpc::rpc_error &t)
    {
        using err_t = std::tuple<int, std::string>;
        auto err = t.get_error().as<err_t>();
        Napi::TypeError::New(env, std::get<1>(err)).ThrowAsJavaScriptException();
    }
    return env.Null();
}

// void DelProject(long long pid);
Napi::Value DelProject(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    try
    {
        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Undefined();
        }

        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
            return env.Undefined();
        }
        auto id = info[0].As<Napi::Number>().Int64Value();
        __c->call("DelProject", id);
    }
    catch (rpc::timeout &t) 
    {
        Napi::TypeError::New(env, "Timeout when call rkdbserver").ThrowAsJavaScriptException();
    }
    catch (rpc::rpc_error &t)
    {
        using err_t = std::tuple<int, std::string>;
        auto err = t.get_error().as<err_t>();
        Napi::TypeError::New(env, std::get<1>(err)).ThrowAsJavaScriptException();
    }
    return env.Undefined();
}

// void SetProjectInfo(long long pid, string const & value);
Napi::Value SetProjectInfo(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    try
    {
        if (info.Length() < 2)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Undefined();
        }

        if (!info[0].IsNumber() || !info[1].IsString())
        {
            Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
            return env.Undefined();
        }
        auto id = info[0].As<Napi::Number>().Int64Value();
        auto pinfo = info[1].As<Napi::String>().Utf8Value();
        __c->call("SetProjectInfo", id, pinfo);
    }
    catch (rpc::timeout &t) 
    {
        Napi::TypeError::New(env, "Timeout when call rkdbserver").ThrowAsJavaScriptException();
    }
    catch (rpc::rpc_error &t)
    {
        using err_t = std::tuple<int, std::string>;
        auto err = t.get_error().as<err_t>();
        Napi::TypeError::New(env, std::get<1>(err)).ThrowAsJavaScriptException();
    }
    return env.Undefined();
}

// void SetProject(long long pid, string const & vlaue);
Napi::Value SetProject(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    try
    {
        if (info.Length() < 2)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Undefined();
        }

        if (!info[0].IsNumber() || !info[1].IsString())
        {
            Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
            return env.Undefined();
        }
        auto id = info[0].As<Napi::Number>().Int64Value();
        auto pinfo = info[1].As<Napi::String>().Utf8Value();
        __c->call("SetProject", id, pinfo);
    }
    catch (rpc::timeout &t) 
    {
        Napi::TypeError::New(env, "Timeout when call rkdbserver").ThrowAsJavaScriptException();
    }
    catch (rpc::rpc_error &t)
    {
        using err_t = std::tuple<int, std::string>;
        auto err = t.get_error().as<err_t>();
        Napi::TypeError::New(env, std::get<1>(err)).ThrowAsJavaScriptException();
    }
    return env.Undefined();
}

// string GetProject(long long pid);
Napi::Value GetProject(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    try
    {
        if (info.Length() < 1)
        {
            Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
            return env.Null();
        }

        if (!info[0].IsNumber())
        {
            Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto id = info[0].As<Napi::Number>().Int64Value();
        auto pj = __c->call("GetProject", id).as<string>();
        Napi::String ret = Napi::String::New(env, pj);
        return ret;
    }
    catch (rpc::timeout &t) 
    {
        Napi::TypeError::New(env, "Timeout when call rkdbserver").ThrowAsJavaScriptException();
    }
    catch (rpc::rpc_error &t)
    {
        using err_t = std::tuple<int, std::string>;
        auto err = t.get_error().as<err_t>();
        Napi::TypeError::New(env, std::get<1>(err)).ThrowAsJavaScriptException();
    }
    return env.Null();
}


Napi::Object Init(Napi::Env env, Napi::Object exports) 
{
    exports.Set(Napi::String::New(env, "OpenRkdb"), Napi::Function::New(env, OpenRkdb));
    exports.Set(Napi::String::New(env, "CloseRkdb"), Napi::Function::New(env, CloseRkdb));
    exports.Set(Napi::String::New(env, "GetProjectInfoList"), Napi::Function::New(env, GetProjectInfoList));
    exports.Set(Napi::String::New(env, "NewProjectInfo"), Napi::Function::New(env, NewProjectInfo));
    exports.Set(Napi::String::New(env, "DelProject"), Napi::Function::New(env, DelProject));
    exports.Set(Napi::String::New(env, "SetProjectInfo"), Napi::Function::New(env, SetProjectInfo));
    exports.Set(Napi::String::New(env, "SetProject"), Napi::Function::New(env, SetProject));
    exports.Set(Napi::String::New(env, "GetProject"), Napi::Function::New(env, GetProject));

    return exports;
}

NODE_API_MODULE(addon, Init)