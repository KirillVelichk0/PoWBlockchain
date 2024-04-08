#!/bin/bash
export CC=/usr/bin/clang
export CXX=/usr/bin/clang++
cmake -DCMAKE_BUILD_TYPE=Release ${USE_VCPKG} -B../build -S..
cd ../build
make all
