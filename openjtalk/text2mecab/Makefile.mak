
CC = cl

CFLAGS = /O2 /Ob2 /Oi /Ot /Oy /GT /GL /TC /MD /D CHARSET_UTF_8 /utf-8
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
