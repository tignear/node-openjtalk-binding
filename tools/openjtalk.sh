set -eu
hts_engine_dir=`pwd`/htsengine
cd openjtalk
./configure --with-hts-engine-header-path=${hts_engine_dir}/include --with-hts-engine-library-path=${hts_engine_dir}/lib --with-charset=UTF-8 --disable-dependency-tracking
make
