
all: char.bin matrix.bin sys.dic unk.dic

char.bin matrix.bin sys.dic unk.dic: naist-jdic.csv matrix.def left-id.def pos-id.def rewrite.def right-id.def char.def unk.def feature.def
	copy _left-id.def left-id.def
	copy _rewrite.def rewrite.def
	copy _pos-id.def pos-id.def
	copy _right-id.def right-id.def
	..\mecab\src\mecab-dict-index.exe -d . -o . -f UTF-8 -t sjis

clean:
	del char.bin matrix.bin sys.dic unk.dic
