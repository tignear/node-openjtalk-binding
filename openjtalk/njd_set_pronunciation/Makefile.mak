
CC = cl

CFLAGS = /O3 /Ob2 /Oi /Ot /Oy /GT /GL /TC /I ../njd /D CHARSET_UTF_8 /utf-8
LFLAGS = /LTCG

CORES = njd_set_pronunciation.obj

all: njd_set_pronunciation.lib

njd_set_pronunciation.lib: $(CORES)
	lib $(LFLAGS) /OUT:$@ $(CORES)

.c.obj:
	$(CC) $(CFLAGS) /c $<

clean:
	del *.lib
	del *.obj
