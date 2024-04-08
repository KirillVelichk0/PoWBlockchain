#!/bin/bash
export CC=emcc
export CXX=em++
export VCPKG_TARGET_TRIPLET=wasm32-emscripten 
cmake -DCMAKE_BUILD_TYPE=Release -DVCPKG_TARGET_TRIPLET=wasm32-emscripten -DCMAKE_CXX_FLAGS="-O3 -s WASM=1" ${USE_EMS} ${USE_VCPKG} -B../build -S..
cd ../build
make all
