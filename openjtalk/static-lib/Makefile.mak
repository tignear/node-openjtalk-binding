
CC = cl

CFLAGS = /O2 /Ob2 /Oi /Ot /Oy /GT /GL /TC /I ..\text2mecab /I ..\mecab\src /I ..\mecab2njd /I ..\njd /I ..\njd_set_pronunciation /I ..\njd_set_digit /I ..\njd_set_accent_phrase /I ..\njd_set_accent_type /I ..\njd_set_unvoiced_vowel /I ..\njd_set_long_vowel /I ..\njd2jpcommon /I ..\jpcommon /I %%HTS_ENGINE_DIR%%\include /I .
LFLAGS = /LTCG

CORES = open_jtalk.obj

all: open_jtalk.lib

open_jtalk.lib: $(CORES)
	lib $(LFLAGS) /OUT:$@ $(CORES)

.c.obj:
	$(CC) $(CFLAGS) /c $<

clean:
	del *.lib
	del *.obj