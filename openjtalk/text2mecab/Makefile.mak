
CC = cl

CFLAGS = /O3 /Ob2 /Oi /Ot /Oy /GT /GL /TC /D CHARSET_UTF_8 /utf-8
LFLAGS = /LTCG

CORES = text2mecab.obj

all: text2mecab.lib

text2mecab.lib: $(CORES)
	lib $(LFLAGS) /OUT:$@ $(CORES)

.c.obj:
	$(CC) $(CFLAGS) /c $<

clean:
	del *.lib
	del *.obj