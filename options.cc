#include <napi.h>
#include <open_jtalk.h>
#include <limits>
#include <optional>

struct Options
{
  std::optional<size_t> sampling_frequency;
  std::optional<size_t> frame_period;
  std::optional<size_t> audio_buffer_size;
  std::optional<double> all_pass_constant;
  std::optional<double> postfiltering_coefficient;
  std::optional<double> speech_speed_rate;
  std::optional<double> additional_half_tone;
  std::optional<double> voiced_unvoiced_threshold;
  std::optional<double> weight_of_GV_for_spectrum;
  std::optional<double> weight_of_GV_for_log_F0;
  std::optional<double> volume_in_dB;
};

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
constexpr auto option(
    const char *key,
    std::optional<U> Options::*m,
    F r,
    void (*config_fn)(Open_JTalk *, U))
{
  auto extract = [key, r, m](Options *options,
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
      *options.*m = d;
    }
    else
    {
      CheckNullish(v, key);
    }
  };
  auto set = [m, config_fn](Open_JTalk *open_jtalk, Options *opt) {
    auto v = *opt.*m;
    if (v)
    {
      config_fn(open_jtalk, *v);
    }
  };
  return std::make_tuple(extract, set);
}

const static constexpr auto int_option_list = {
    option<int64_t>("sampling_frequency", &Options::sampling_frequency, range<int64_t>(1), &Open_JTalk_set_sampling_frequency),
    option<int64_t>("frame_period", &Options::frame_period, range<int64_t>(1), &Open_JTalk_set_fperiod),
    option<int64_t>("audio_buffer_size", &Options::audio_buffer_size, range<int64_t>(0), &Open_JTalk_set_audio_buff_size),
};
const static constexpr auto double_option_list = {
    option("all_pass_constant", &Options::all_pass_constant, range(0.0, 1.0), &Open_JTalk_set_alpha),
    option("postfiltering_coefficient", &Options::postfiltering_coefficient, range(0.0,1.0), &Open_JTalk_set_beta),
    option("speech_speed_rate", &Options::speech_speed_rate, range(0.0), &Open_JTalk_set_speed),
    option("additional_half_tone", &Options::additional_half_tone, range<double>(), &Open_JTalk_add_half_tone),
    option<Napi::Number, double>("voiced_unvoiced_threshold", &Options::voiced_unvoiced_threshold, range(0.0, 1.0), [](Open_JTalk *open_jtalk, double value) {
      Open_JTalk_set_msd_threshold(open_jtalk, 1, value);
    }),
    option<Napi::Number, double>("weight_of_GV_for_spectrum", &Options::weight_of_GV_for_spectrum, range(0.0), [](Open_JTalk *open_jtalk, double value) {
      Open_JTalk_set_gv_weight(open_jtalk, 0, value);
    }),
    option<Napi::Number, double>("weight_of_GV_for_log_F0", &Options::weight_of_GV_for_log_F0, range(0.0), [](Open_JTalk *open_jtalk, double value) {
      Open_JTalk_set_gv_weight(open_jtalk, 1, value);
    }),
    option("volume_in_dB", &Options::volume_in_dB, range<double>(), &Open_JTalk_set_volume),
};

template <size_t idx, class Dst, class Src>
inline void OptionsLoop(Dst dst, Src src)
{
  for (auto &entry : int_option_list)
  {
    std::get<idx>(entry)(dst, src);
  }
  for (auto &entry : double_option_list)
  {
    std::get<idx>(entry)(dst, src);
  }
}

void ExtractOptions(Options *options, Napi::Object &js_options)
{
  OptionsLoop<0>(options, js_options);
}
void SetOptions(Open_JTalk *open_jtalk, Options *options)
{
  OptionsLoop<1>(open_jtalk, options);
}