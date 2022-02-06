#include <napi.h>
#include <open_jtalk.hpp>
#include <limits>

/**compatibility of c++14**/
#if __cplusplus >= 201703L
/**c++17**/
#include <optional>
using std::optional;
#else
/**c++14**/
#include "nonstd/optional.hpp"
using nonstd::optional;
#endif
struct Options
{
  optional<size_t> sampling_frequency;
  optional<size_t> frame_period;
  optional<size_t> audio_buffer_size;
  optional<double> all_pass_constant;
  optional<double> postfiltering_coefficient;
  optional<double> speech_speed_rate;
  optional<double> additional_half_tone;
  optional<double> voiced_unvoiced_threshold;
  optional<double> weight_of_GV_for_spectrum;
  optional<double> weight_of_GV_for_log_F0;
  optional<double> volume_in_dB;
};

#if __cplusplus >= 201703L
#define option_return auto
#else
#include <array>
#include <functional>

#define option_return std::tuple<std::function<void(Options &options, Napi::Object &js_options)>, std::function<void(Open_JTalk *open_jtalk,const Options &opt)>>

template <size_t N>
using option_list = std::array<option_return, N>;

template <typename... Args>
static constexpr inline option_list<sizeof...(Args)> make_array(Args &&... args)
{
  return option_list<sizeof...(Args)>{std::forward<Args>(args)...};
}
#endif

using namespace std::literals::string_literals;
void CheckNullish(Napi::Value &value, const std::string &key)
{
  if (!(value.IsNull() || value.IsUndefined()))
  {
    throw Napi::TypeError::New(value.Env(), "Invalid "s + key + " type."s);
  }
}

template <class T>
constexpr auto range(T min = std::numeric_limits<T>::min(), T max = std::numeric_limits<T>::max())
{
  return [min, max](T value) {
    return min <= value && value <= max;
  };
}

template <class T = Napi::Number, class U = T, class F>
constexpr option_return option(
    const char *key,
    optional<U> Options::*m,
    F r,
    void (*config_fn)(Open_JTalk *, U))
{
  auto extract = [key, r, m](Options &options,
                             Napi::Object &js_options) {
    Napi::Env env = js_options.Env();

    auto v = js_options.Get(key);
    if (v.IsNumber())
    {
      T d = v.As<Napi::Number>();
      if (!r(d))
      {
        throw Napi::TypeError::New(env, key + " is out of range."s);
      }
      options.*m = d;
    }
    else
    {
      CheckNullish(v, key);
    }
  };
  auto set = [m, config_fn](Open_JTalk *open_jtalk, const Options &opt) {
    auto v = opt.*m;
    if (v)
    {
      config_fn(open_jtalk, *v);
    }
  };
  return std::make_tuple(extract, set);
}

#if __cplusplus >= 201703L
const static constexpr auto int_option_list = {
#include "int_options"
};
const static constexpr auto double_option_list = {
#include "double_options"
};
#endif

template <size_t idx, class Dst, class Src>
inline void OptionsLoop(Dst &dst, Src src)
{
#if __cplusplus < 201703L
  auto int_option_list = make_array(
#include "int_options"
  );
  auto double_option_list = make_array(
#include "double_options"
  );
#endif
  for (auto &entry : int_option_list)
  {
    std::get<idx>(entry)(dst, src);
  }
  for (auto &entry : double_option_list)
  {
    std::get<idx>(entry)(dst, src);
  }
}

void ExtractOptions(Options &options, Napi::Object &js_options)
{
  OptionsLoop<0>(options, js_options);
}
void SetOptions(Open_JTalk *open_jtalk,const Options &options)
{
  OptionsLoop<1>(open_jtalk, options);
}