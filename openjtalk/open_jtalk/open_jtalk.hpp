#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

/* Main headers */
#include "mecab.h"
#include "njd.h"
#include "jpcommon.h"
#include "HTS_engine.h"

typedef struct _Open_JTalk {
   MeCab::Mecab mecab;
   NJD njd;
   JPCommon jpcommon;
   HTS_Engine engine;
} Open_JTalk;
void Open_JTalk_initialize(Open_JTalk *open_jtalk);
void Open_JTalk_clear(Open_JTalk *open_jtalk);
int Open_JTalk_load(Open_JTalk *open_jtalk,void *voice_data,size_t length_of_voice_data,const MeCab::ViterbiOptions& viterbi_options);
void Open_JTalk_set_sampling_frequency(Open_JTalk *open_jtalk, size_t i);
void Open_JTalk_set_fperiod(Open_JTalk *open_jtalk, size_t i);
void Open_JTalk_set_alpha(Open_JTalk *open_jtalk, double f);
void Open_JTalk_set_beta(Open_JTalk *open_jtalk, double f);
void Open_JTalk_set_speed(Open_JTalk *open_jtalk, double f);
void Open_JTalk_add_half_tone(Open_JTalk *open_jtalk, double f);
void Open_JTalk_set_msd_threshold(Open_JTalk *open_jtalk, size_t i, double f);
void Open_JTalk_set_gv_weight(Open_JTalk *open_jtalk, size_t i, double f);
void Open_JTalk_set_volume(Open_JTalk *open_jtalk, double f);
void Open_JTalk_set_audio_buff_size(Open_JTalk *open_jtalk, size_t i);
int Open_JTalk_synthesis(Open_JTalk *open_jtalk, const char *txt, signed short **pcm, size_t *length_of_pcm);