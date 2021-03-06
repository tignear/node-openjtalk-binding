#include "open_jtalk.hpp"
/* Sub headers */
#include "text2mecab.h"
#include "mecab2njd.h"
#include "njd_set_pronunciation.h"
#include "njd_set_digit.h"
#include "njd_set_accent_phrase.h"
#include "njd_set_accent_type.h"
#include "njd_set_unvoiced_vowel.h"
#include "njd_set_long_vowel.h"
#include "njd2jpcommon.h"
#define MAXBUFLEN 1024

void Open_JTalk_initialize(Open_JTalk *open_jtalk)
{
  MeCab::Mecab_initialize(&open_jtalk->mecab);
  NJD_initialize(&open_jtalk->njd);
  JPCommon_initialize(&open_jtalk->jpcommon);
  HTS_Engine_initialize(&open_jtalk->engine);
}

void Open_JTalk_clear(Open_JTalk *open_jtalk)
{
  MeCab::Mecab_clear(&open_jtalk->mecab);
  NJD_clear(&open_jtalk->njd);
  JPCommon_clear(&open_jtalk->jpcommon);
  HTS_Engine_clear(&open_jtalk->engine);
}

int Open_JTalk_load(Open_JTalk *open_jtalk, void *voice_data, size_t length_of_voice_data, const MeCab::ViterbiOptions &viterbi_options)
{
  if (MeCab::Mecab_load(&open_jtalk->mecab, viterbi_options) != TRUE)
  {
    Open_JTalk_clear(open_jtalk);
    return 1;
  }
  if (HTS_Engine_load(&open_jtalk->engine, &voice_data, &length_of_voice_data, 1) != TRUE)
  {
    Open_JTalk_clear(open_jtalk);
    return 2;
  }
  if (strcmp(HTS_Engine_get_fullcontext_label_format(&open_jtalk->engine), "HTS_TTS_JPN") != 0)
  {
    Open_JTalk_clear(open_jtalk);
    return 3;
  }
  return 0;
}

void Open_JTalk_set_sampling_frequency(Open_JTalk *open_jtalk, size_t i)
{
  HTS_Engine_set_sampling_frequency(&open_jtalk->engine, i);
}

void Open_JTalk_set_fperiod(Open_JTalk *open_jtalk, size_t i)
{
  HTS_Engine_set_fperiod(&open_jtalk->engine, i);
}

void Open_JTalk_set_alpha(Open_JTalk *open_jtalk, double f)
{
  HTS_Engine_set_alpha(&open_jtalk->engine, f);
}

void Open_JTalk_set_beta(Open_JTalk *open_jtalk, double f)
{
  HTS_Engine_set_beta(&open_jtalk->engine, f);
}

void Open_JTalk_set_speed(Open_JTalk *open_jtalk, double f)
{
  HTS_Engine_set_speed(&open_jtalk->engine, f);
}

void Open_JTalk_add_half_tone(Open_JTalk *open_jtalk, double f)
{
  HTS_Engine_add_half_tone(&open_jtalk->engine, f);
}

void Open_JTalk_set_msd_threshold(Open_JTalk *open_jtalk, size_t i, double f)
{
  HTS_Engine_set_msd_threshold(&open_jtalk->engine, i, f);
}

void Open_JTalk_set_gv_weight(Open_JTalk *open_jtalk, size_t i, double f)
{
  HTS_Engine_set_gv_weight(&open_jtalk->engine, i, f);
}

void Open_JTalk_set_volume(Open_JTalk *open_jtalk, double f)
{
  HTS_Engine_set_volume(&open_jtalk->engine, f);
}

void Open_JTalk_set_audio_buff_size(Open_JTalk *open_jtalk, size_t i)
{
  HTS_Engine_set_audio_buff_size(&open_jtalk->engine, i);
}

int Open_JTalk_synthesis(Open_JTalk *open_jtalk, const char *txt, signed short **pcm, size_t *length_of_pcm)
{
  int result = 0;
  char *buff = (char *)malloc(strlen(txt) * 4 + 1);

  text2mecab(buff, txt);
  MeCab::Mecab_analysis(&open_jtalk->mecab, buff);
  mecab2njd(&open_jtalk->njd, MeCab::Mecab_get_feature(&open_jtalk->mecab),
            MeCab::Mecab_get_size(&open_jtalk->mecab));
  njd_set_pronunciation(&open_jtalk->njd);
  njd_set_digit(&open_jtalk->njd);
  njd_set_accent_phrase(&open_jtalk->njd);
  njd_set_accent_type(&open_jtalk->njd);
  njd_set_unvoiced_vowel(&open_jtalk->njd);
  njd_set_long_vowel(&open_jtalk->njd);
  njd2jpcommon(&open_jtalk->jpcommon, &open_jtalk->njd);
  JPCommon_make_label(&open_jtalk->jpcommon);
  if (JPCommon_get_label_size(&open_jtalk->jpcommon) > 2)
  {
    if (HTS_Engine_synthesize_from_strings(&open_jtalk->engine, JPCommon_get_label_feature(&open_jtalk->jpcommon),
                                           JPCommon_get_label_size(&open_jtalk->jpcommon)) == TRUE)
      result = 1;
    HTS_Engine_save_pcm(&open_jtalk->engine, pcm, length_of_pcm);
    HTS_Engine_refresh(&open_jtalk->engine);
  }
  JPCommon_refresh(&open_jtalk->jpcommon);
  NJD_refresh(&open_jtalk->njd);
  MeCab::Mecab_refresh(&open_jtalk->mecab);
  free(buff);

  return result;
}
