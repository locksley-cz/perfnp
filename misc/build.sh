#/bin/bash -e

rm -rf build
cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Release
make -sj4 -Cbuild
build/tests
