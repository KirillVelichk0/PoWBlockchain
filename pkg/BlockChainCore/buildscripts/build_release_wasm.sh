#!/bin/bash
export CC=/path/to/emsdk/upstream/emscripten/emcc
export CXX=/path/to/emsdk/upstream/emscripten/em++
VCPKG_TARGET_TRIPLET=wasm32-emscripten 
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-O3 -s WASM=1" ${USE_VCPKG} -B../build -S..
cd ../build
make all
