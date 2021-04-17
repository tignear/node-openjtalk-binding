
CC = cl

CFLAGS = /O3 /Ob2 /Oi /Ot /Oy /GT /GL /TC /I ..\text2mecab /I ..\mecab\src /I ..\mecab2njd /I ..\njd /I ..\njd_set_pronunciation /I ..\njd_set_digit /I ..\njd_set_accent_phrase /I ..\njd_set_accent_type /I ..\njd_set_unvoiced_vowel /I ..\njd_set_long_vowel /I ..\njd2jpcommon /I ..\jpcommon /I %%HTS_ENGINE_DIR%%\include /I .
LFLAGS = /LTCG

LIBS = ..\text2mecab\text2mecab.lib ..\mecab\src\mecab.lib ..\mecab2njd\mecab2njd.lib ..\njd\njd.lib ..\njd_set_pronunciation\njd_set_pronunciation.lib ..\njd_set_digit\njd_set_digit.lib ..\njd_set_accent_phrase\njd_set_accent_phrase.lib ..\njd_set_accent_type\njd_set_accent_type.lib ..\njd_set_unvoiced_vowel\njd_set_unvoiced_vowel.lib ..\njd_set_long_vowel\njd_set_long_vowel.lib ..\njd2jpcommon\njd2jpcommon.lib ..\jpcommon\jpcommon.lib ..\..\htsengine\lib\hts_engine_API.lib winmm.lib Advapi32.lib

CORES = open_jtalk.obj 

all: open_jtalk.lib

open_jtalk.lib: $(CORES)
	lib $(LFLAGS) /OUT:$@ $(CORES) $(LIBS)

.c.obj:
	$(CC) $(CFLAGS) /c $<

clean:
	del *.lib
	del *.obj