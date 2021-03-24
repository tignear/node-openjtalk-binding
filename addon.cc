#include <napi.h>
#include <thread>
#include <open_jtalk.h>
#include <variant>
#include <string>
#include "options.cc"
using Context = Napi::Reference<Napi::Value>;
struct CShortArray
{
  size_t length;
  signed short *value;
};
using DataType = std::variant<CShortArray, const char *>;
void CallJs(Napi::Env env, Napi::Function callback, Context *context, DataType *data);
using TSFN = Napi::TypedThreadSafeFunction<Context, DataType, CallJs>;
using FinalizerDataType = void;

void LoadArguments(const Napi::CallbackInfo &info, std::string &text, std::string &dn_dict, std::string &fn_voice, Options *options)
{
  Napi::Env env = info.Env();
  if (info.Length() < 3)
  {
    throw Napi::TypeError::New(env, "Expected three arguments.");
  }
  if (!info[0].IsFunction())
  {
    throw Napi::TypeError::New(env, "Expected callback to be function.");
  }
  if (!info[1].IsString())
  {
    throw Napi::TypeError::New(env, "Expected text to be string.");
  }
  if (!info[2].IsObject())
  {
    throw Napi::TypeError::New(env, "Expected options to be object.");
  }
  auto js_options = info[2].As<Napi::Object>();

  if (!js_options.Has("dictionary"))
  {
    throw Napi::TypeError::New(env, "Expected options to have dictionary.");
  }
  auto dictionary_js_value = js_options.Get("dictionary");
  if (!dictionary_js_value.IsString())
  {
    throw Napi::TypeError::New(env, "Expected dictionary to be string.");
  }
  if (!js_options.Has("htsvoice"))
  {
    throw Napi::TypeError::New(env, "Expected options to have htsvoice.");
  }
  auto htsvoice_js_value = js_options.Get("htsvoice");
  if (!htsvoice_js_value.IsString())
  {
    throw Napi::TypeError::New(env, "Expected htsvoice to be string.");
  }

  text = info[1].As<Napi::String>().Utf8Value();
  dn_dict = dictionary_js_value.As<Napi::String>();
  fn_voice = htsvoice_js_value.As<Napi::String>();
  ExtractOptions(options, js_options);
}
Napi::Value Synthesis(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  std::string text;
  std::string dn_dict;
  std::string fn_voice;
  Options options;
  LoadArguments(info, text, dn_dict, fn_voice, &options);
  Context *context = new Napi::Reference<Napi::Value>(Napi::Persistent(info.This()));
  TSFN tsfn = TSFN::New(
      env,
      info[0].As<Napi::Function>(),
      "Synthesis Callback",
      0,
      1,
      context,
      [](Napi::Env, FinalizerDataType *,
         Context *ctx) {
        delete ctx;
      });

  auto lambda = [tsfn, dn_dict, fn_voice, text, options]() mutable {
    Open_JTalk open_jtalk;

    Open_JTalk_initialize(&open_jtalk);

    int code = Open_JTalk_load(&open_jtalk, dn_dict.c_str(), fn_voice.c_str());
    if (code)
    {
      switch (code)
      {
      case 1:
        tsfn.BlockingCall(new DataType("Failed to load OpenJTalk.The dictionary is invalid."));
        break;
      case 2:
        tsfn.BlockingCall(new DataType("Failed to load OpenJTalk.The htsvoice is invalid."));
        break;
      case 3:
        tsfn.BlockingCall(new DataType("Failed to load OpenJTalk.The htsvoice is invalid(expected FULLCONTEXT_FORMAT to be HTS_TTS_JPN)."));
      }
      tsfn.Release();
      return;
    }
    SetOptions(&open_jtalk, &options);

    signed short *pcm;
    size_t length_of_pcm;
    if (Open_JTalk_synthesis(&open_jtalk, text.c_str(), &pcm, &length_of_pcm) != TRUE)
    {
      Open_JTalk_clear(&open_jtalk);
      tsfn.BlockingCall(new DataType("Synthesis failed."));
      tsfn.Release();
      return;
    }

    tsfn.BlockingCall(new DataType(CShortArray{
        length_of_pcm, pcm}));
    tsfn.Release();
  };

  auto thread = std::thread(lambda);
  thread.detach();
  return env.Undefined();
}

void CallJs(Napi::Env env, Napi::Function callback, Context *context,
            DataType *data)
{
  // Is the JavaScript environment still available to call into, eg. the TSFN is
  // not aborted
  if (env != nullptr)
  {
    // On N-API 5+, the `callback` parameter is optional; however, this example
    // does ensure a callback is provided.
    if (callback != nullptr)
    {
      if (std::holds_alternative<CShortArray>(*data))
      {
        auto sa = std::get<CShortArray>(*data);
        auto buffer = Napi::Buffer<signed short>::New(
            env, sa.value, sa.length, [](Napi::Env, signed short *pcm) {
              free(pcm);
            });
        callback.Call(context->Value(), {env.Null(), buffer});
      }
      else
      {
        auto msg = std::get<const char *>(*data);
        callback.Call(context->Value(), {Napi::Error::New(env, msg).Value()});
      }
    }
  }
  if (data != nullptr)
  {
    // We're finished with the data.
    delete data;
  }
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set("synthesis", Napi::Function::New(env, Synthesis));
  return exports;
}

NODE_API_MODULE(addon, Init)