
CC = cl

CFLAGS = /O2 /Ob2 /Oi /Ot /Oy /GT /GL /TC /MD /I ../njd /D CHARSET_UTF_8 /utf-8
LFLAGS = /LTCG

CORES = njd_set_accent_phrase.obj

all: njd_set_accent_phrase.lib

njd_set_accent_phrase.lib: $(CORES)
	lib $(LFLAGS) /OUT:$@ $(CORES)

.c.obj:
	$(CC) $(CFLAGS) /c $<

clean:
	del *.lib
	del *.obj
