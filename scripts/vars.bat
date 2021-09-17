@echo off
set CC=clang
set CXX=clang++
set CFLAGS=-nostdlib
set CXXFLAGS=-nostdlib
if "%GDSTL_TARGET%" == "windows" (
    set CFLAGS=-m32 %CFLAGS%
    set CXXFLAGS=-m32 %CXXFLAGS%
)
set GDSTL_NDK=path/to/ndk/root/with/forward/slashes
set GDSTL_PATH=%CD%