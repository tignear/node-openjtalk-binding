set -eu
cd htsengine
rm -f Makefile
./configure --disable-dependency-tracking
make