set -eu
hts_engine_dir=`pwd`/htsengine
cd openjtalk
rm -f Makefile
aclocal
automake -a -c
autoconf
./configure --with-hts-engine-header-path=${hts_engine_dir}/include --with-hts-engine-library-path=${hts_engine_dir}/lib --disable-dependency-tracking
make clean
make
