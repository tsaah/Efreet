md build-debug
md bin-debug
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=./bin-debug -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build build-debug --config Debug -j10
cmake --install build-debug --config Debug