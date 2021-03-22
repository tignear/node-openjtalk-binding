set -eu
cd htsengine
rm -f Makefile
touch ChangeLog
aclocal
automake -a -c
autoconf
./configure
make