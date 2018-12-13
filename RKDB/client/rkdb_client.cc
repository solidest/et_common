
#include <node_api.h>
#include <assert.h>
#include <string>

#include "rpc/client.h"
#include "rpc/rpc_error.h"

using namespace std;

// string GetProjectInfoList();
// long long NewProjectInfo(string const & value);
// void SetProjectInfo(long long pid, string const & value);
// void SetProject(long long pid, string const & vlaue);
// string GetProject(long long pid);
// void DelProject(long long pid);
//static rpc::client *__c;

napi_value OpenRkdb(napi_env env, napi_callback_info info) 
{
    return nullptr;
}

napi_value CloseRkdb(napi_env env, napi_callback_info info) 
{
    return nullptr;
}


napi_value GetProjectInfoList(napi_env env, napi_callback_info info) 
{
  rpc::client c("127.0.0.1", 8000);

  auto pl = c.call("GetProjectInfoList").as<string>();

  napi_value ret;
  napi_status status = napi_create_string_utf8(env, "test!", 5,/*pl.c_str(), pl.size()*/ &ret);
  assert(status == napi_ok);
  return ret;
}

napi_value NewProjectInfo(napi_env env, napi_callback_info info) 
{
  return nullptr;
}

#define DECLARE_NAPI_METHOD(name, func) { name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_property_descriptor desc[] = {
      DECLARE_NAPI_METHOD("GetProjectInfoList", GetProjectInfoList),
      DECLARE_NAPI_METHOD("NewProjectInfo", NewProjectInfo),
  };
  status =
      napi_define_properties(env, exports, sizeof(desc) / sizeof(*desc), desc);
  assert(status == napi_ok);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
