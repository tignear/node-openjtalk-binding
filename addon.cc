#include <napi.h>
#include <thread>
#include <open_jtalk.h>
#include <string>
#include "options.cc"
#include "thread_pool.hpp"

/**compatibility of c++14**/
#if __cplusplus >= 201703L
/**c++17**/
#include <variant>
using std::get;
using std::holds_alternative;
using std::variant;
#else
/**c++14**/
#include "nonstd/variant.hpp"
using nonstd::get;
using nonstd::holds_alternative;
using nonstd::variant;
#endif

using Context = Napi::Reference<Napi::Value>;

struct Wave
{
  size_t length;
  signed short *value;
  size_t sampling_frequency;
};
using DataType = variant<Wave, const char *>;
void CallJs(Napi::Env env, Napi::Function callback, Context *context, DataType *data);
using TSFN = Napi::TypedThreadSafeFunction<Context, DataType, CallJs>;
using FinalizerDataType = void;
void taskFunc(TSFN tsfn, const std::string &dn_dict, void *voice_data, size_t length_of_voice_data, const std::string &text, const Options &options)
{
  Open_JTalk open_jtalk;

  Open_JTalk_initialize(&open_jtalk);

  int code = Open_JTalk_load(&open_jtalk, dn_dict.c_str(), voice_data, length_of_voice_data);
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
  SetOptions(&open_jtalk, options);

  signed short *pcm;
  size_t length_of_pcm;
  if (Open_JTalk_synthesis(&open_jtalk, text.c_str(), &pcm, &length_of_pcm) != TRUE)
  {
    Open_JTalk_clear(&open_jtalk);
    tsfn.BlockingCall(new DataType("Synthesis failed."));
    tsfn.Release();
    return;
  }

  tsfn.BlockingCall(new DataType(Wave{
      length_of_pcm, pcm, open_jtalk.engine.condition.sampling_frequency}));
  tsfn.Release();
}
void LoadArguments(const Napi::CallbackInfo &info, std::string &text, std::string &dn_dict, void *&voice_data, size_t &length_of_voice_data, Options *options)
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
  if (!htsvoice_js_value.IsArrayBuffer())
  {
    throw Napi::TypeError::New(env, "Expected htsvoice to be ArrayBuffer.");
  }

  text = info[1].As<Napi::String>().Utf8Value();
  dn_dict = dictionary_js_value.As<Napi::String>();
  auto buff = htsvoice_js_value.As<Napi::ArrayBuffer>();
  voice_data = buff.Data();
  length_of_voice_data = buff.ByteLength();
  ExtractOptions(options, js_options);
}
thread_pool pool(std::thread::hardware_concurrency() * 2);
Napi::Value Synthesis(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  std::string text;
  std::string dn_dict;
  void *voice_data;
  size_t length_of_voice_data;
  Options options;
  LoadArguments(info, text, dn_dict, voice_data, length_of_voice_data, &options);
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

  pool.submit(taskFunc, tsfn, dn_dict, voice_data, length_of_voice_data, text, options);
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
      if (holds_alternative<Wave>(*data))
      {
        auto wave = get<Wave>(*data);
        auto buffer = Napi::Buffer<signed short>::New(
            env, wave.value, wave.length, [](Napi::Env, signed short *pcm) {
              free(pcm);
            });
        callback.Call(context->Value(), {env.Null(), buffer, Napi::Number::New(env, wave.sampling_frequency)});
      }
      else
      {
        auto msg = get<const char *>(*data);
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