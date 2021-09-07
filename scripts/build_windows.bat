@echo off
set CC=clang
set CFLAGS=-m32 -nostdlib
set CXX=clang++
set CXXFLAGS=-m32 -nostdlib
mkdir build
cd build
cmake ^
 -DCMAKE_BUILD_TYPE=Release^
 -G "Ninja"^
 -DGDSTL_TARGET=windows^
 ..
ninja gdstl
cd ..