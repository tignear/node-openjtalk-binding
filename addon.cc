#define NAPI_VERSION 5
#include <napi.h>
#include <thread>
#include <open_jtalk.hpp>
#include <string>
#include "options.cc"
#include "thread_pool/ThreadPool.h"

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
void CallJs(Napi::Env env, Napi::Function callback, Context *context,
            DataType *data)
{

  if (env != nullptr)
  {
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
template <class T>
void CallRelease(Napi::Env env, Napi::Function callback, std::nullptr_t *, Napi::Reference<T> *data)
{
  delete data;
}
using ResultTSFN = Napi::TypedThreadSafeFunction<Context, DataType, CallJs>;
template <class T>
using ReleaseTSFN = Napi::TypedThreadSafeFunction<std::nullptr_t, Napi::Reference<T>, CallRelease<T>>;
using FinalizerDataType = void;

std::shared_ptr<char> createReferenceSharedPtr(Napi::Env env, Napi::ArrayBuffer buf)
{
  char *data = reinterpret_cast<char *>(buf.Data());
  Napi::Reference<Napi::ArrayBuffer> *buf_ref = new Napi::Reference<Napi::ArrayBuffer>(Napi::Persistent(buf));
  ReleaseTSFN<Napi::ArrayBuffer> tsfn = ReleaseTSFN<Napi::ArrayBuffer>::New(
      env,
      "Release ArrayBuffer",
      1, 1, nullptr);
  return std::shared_ptr<char>(data, [tsfn, buf_ref](char *) mutable {
    tsfn.NonBlockingCall(buf_ref);
    tsfn.Release();
  });
}

void taskFunc(
    ResultTSFN tsfn,
    ReleaseTSFN<Napi::ArrayBuffer> releaseBuffer,
    Napi::Reference<Napi::ArrayBuffer> *buffer_ref,
    const std::string &dn_dict,
    void *voice_data,
    size_t length_of_voice_data,
    const std::string &text,
    const Options &options,
    const MeCab::TokenizerOpenFromMemoryOptions &tokenizer_options)
{
  Open_JTalk open_jtalk;

  Open_JTalk_initialize(&open_jtalk);

  int code = Open_JTalk_load(
      &open_jtalk,
      dn_dict.c_str(),
      voice_data,
      length_of_voice_data,
      tokenizer_options);
  releaseBuffer.NonBlockingCall(buffer_ref);
  releaseBuffer.Release();
  if (code)
  {
    switch (code)
    {
    case 1:
      tsfn.NonBlockingCall(new DataType("Failed to load OpenJTalk.The dictionary is invalid."));
      break;
    case 2:
      tsfn.NonBlockingCall(new DataType("Failed to load OpenJTalk.The htsvoice is invalid."));
      break;
    case 3:
      tsfn.NonBlockingCall(new DataType("Failed to load OpenJTalk.The htsvoice is invalid(expected FULLCONTEXT_FORMAT to be HTS_TTS_JPN)."));
    }
    tsfn.Release();
    Open_JTalk_clear(&open_jtalk);
    return;
  }
  SetOptions(&open_jtalk, options);

  signed short *pcm;
  size_t length_of_pcm;
  if (Open_JTalk_synthesis(&open_jtalk, text.c_str(), &pcm, &length_of_pcm) != TRUE)
  {
    tsfn.NonBlockingCall(new DataType("Synthesis failed."));
    tsfn.Release();
    Open_JTalk_clear(&open_jtalk);
    return;
  }

  tsfn.NonBlockingCall(new DataType(Wave{
      length_of_pcm, pcm, open_jtalk.engine.condition.sampling_frequency}));
  tsfn.Release();
  Open_JTalk_clear(&open_jtalk);
}
void SetEntryToOption(Napi::Env env, Napi::ArrayBuffer buf, MeCab::TokenizerOpenFromMemoryOptionsData &d)
{
  d.data = createReferenceSharedPtr(env, buf);
  d.size = buf.ByteLength();
}
void LoadDictionaryOptions(Napi::Env env, const Napi::Object &js_dictionary, MeCab::TokenizerOpenFromMemoryOptions &tokenizer_options)
{
  auto js_unkdic = js_dictionary.Get("unkdic");
  if (!js_unkdic.IsArrayBuffer())
  {
    throw Napi::TypeError::New(env, "Expected dictionary.unkdic to be ArrayBuffer.");
  }
  auto js_sysdic = js_dictionary.Get("sysdic");
  if (!js_sysdic.IsArrayBuffer())
  {
    throw Napi::TypeError::New(env, "Expected dictionary.sysdic to be ArrayBuffer.");
  }
  auto js_property = js_dictionary.Get("property");
  if (!js_property.IsArrayBuffer())
  {
    throw Napi::TypeError::New(env, "Expected dictionary.property to be ArrayBuffer.");
  }
  SetEntryToOption(env, js_unkdic.As<Napi::ArrayBuffer>(), tokenizer_options.unkdic);
  SetEntryToOption(env, js_sysdic.As<Napi::ArrayBuffer>(), tokenizer_options.sysdic);
  SetEntryToOption(env, js_property.As<Napi::ArrayBuffer>(), tokenizer_options.property);
}
void LoadArguments(
    const Napi::CallbackInfo &info,
    std::string &text,
    std::string &dn_dict,
    Napi::ArrayBuffer &voice_array_buff,
    Options &options,
    MeCab::TokenizerOpenFromMemoryOptions &tokenizer_options)
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
  if (!dictionary_js_value.IsObject())
  {
    throw Napi::TypeError::New(env, "Expected dictionary to be object.");
  }
  auto js_dictionary = dictionary_js_value.As<Napi::Object>();
  dn_dict = js_dictionary.Get("dir").As<Napi::String>().Utf8Value();
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
  voice_array_buff = htsvoice_js_value.As<Napi::ArrayBuffer>();
  LoadDictionaryOptions(env, js_dictionary, tokenizer_options);
  ExtractOptions(options, js_options);
}
ThreadPool pool(std::thread::hardware_concurrency() * 2);
Napi::Value Synthesis(const Napi::CallbackInfo &info)
{
  Napi::Env env = info.Env();

  std::string text;
  std::string dn_dict;
  Napi::ArrayBuffer voice_array_buff;
  Options options;
  MeCab::TokenizerOpenFromMemoryOptions tokenizer_options;

  LoadArguments(info, text, dn_dict, voice_array_buff, options, tokenizer_options);
  void *voice_data = voice_array_buff.Data();
  size_t length_of_voice_data = voice_array_buff.ByteLength();
  Context *context = new Context(Napi::Persistent(info.This()));
  auto voice_array_buff_ref = new Napi::Reference<Napi::ArrayBuffer>(Napi::Persistent(voice_array_buff));
  ResultTSFN tsfn = ResultTSFN::New(
      env,
      info[0].As<Napi::Function>(),
      "Synthesis Callback",
      1,
      1,
      context,
      [](Napi::Env, FinalizerDataType *,
         Context *ctx) {
        delete ctx;
      });
  auto rtsfn = ReleaseTSFN<Napi::ArrayBuffer>::New(
      env,
      "Release htsvoice ArrayBuffer",
      1, 1, nullptr);
  pool.AddTask(taskFunc, tsfn, rtsfn, voice_array_buff_ref, std::move(dn_dict), voice_data, length_of_voice_data, std::move(text), std::move(options), std::move(tokenizer_options));
  return env.Undefined();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
  exports.Set("synthesis", Napi::Function::New(env, Synthesis));
  return exports;
}

NODE_API_MODULE(addon, Init)