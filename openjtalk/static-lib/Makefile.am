
EXTRA_DIST = Makefile.mak

DISTCLEANFILES = *.log *.out *~

MAINTAINERCLEANFILES = Makefile.in

AM_CPPFLAGS = -I @top_srcdir@/text2mecab \
              -I @top_srcdir@/mecab/src \
              -I @top_srcdir@/mecab2njd \
              -I @top_srcdir@/njd \
              -I @top_srcdir@/njd_set_pronunciation \
              -I @top_srcdir@/njd_set_digit \
              -I @top_srcdir@/njd_set_accent_phrase \
              -I @top_srcdir@/njd_set_accent_type \
              -I @top_srcdir@/njd_set_unvoiced_vowel \
              -I @top_srcdir@/njd_set_long_vowel \
              -I @top_srcdir@/njd2jpcommon \
              -I @top_srcdir@/jpcommon \
              -I @HTS_ENGINE_HEADER_DIR@ \
              -fPIC

noinst_LIBRARIES = libOpenJTalk.a

libOpenJTalk_a_SOURCES = open_jtalk.c
