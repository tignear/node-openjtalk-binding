set -eu
cd htsengine
rm -f Makefile
aclocal
automake -a -c
autoconf
./configure --disable-dependency-tracking
make clean
make
