
// Napi::Value HookLogOut(const Napi::CallbackInfo& info)
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

//     __callback = info[0].As<Napi::Function>();
//     __callback.

//     return env.Undefined();
// }

// Napi::Object Init(Napi::Env env, Napi::Object exports) 
// {
//     exports.Set(Napi::String::New(env, "HookLogOut"), Napi::Function::New(env, HookLogOut));

//     return exports;
// }

// NODE_API_MODULE(addon, Init)