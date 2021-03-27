cd ../

$env:HTS_ENGINE_DIR="$(pwd)/htsengine"
echo $env:HTS_ENGINE_DIR
cd openjtalk
nmake /F Makefile.mak clean
nmake /F Makefile.mak