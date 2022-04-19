md build-release
md bin-release
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=./bin-release -DCMAKE_VERBOSE_MAKEFILE=ON
cmake --build build-release --config Release -j10
cmake --install build-release --config Release