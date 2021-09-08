@echo off
set GDSTL_TARGET=android
:: Init vars
call .\scripts\vars.bat
:: Delete existing folders
if exist %GDSTL_PATH%\build\android-armeabi-v7a\ (
    rmdir /s /q %GDSTL_PATH%\build\android-armeabi-v7a
)
if exist %GDSTL_PATH%\build\android-x86\ (
    rmdir /s /q %GDSTL_PATH%\build\android-x86
)
if exist %GDSTL_PATH%\build\android-armeabi\ (
    rmdir /s /q %GDSTL_PATH%\build\android-armeabi
)
:: Generate build files (armeabi)
mkdir %GDSTL_PATH%\build\android-armeabi
cd  %GDSTL_PATH%\build\android-armeabi
cmake ^
 -DCMAKE_BUILD_TYPE=Release^
 -DCMAKE_SYSTEM_NAME=Android^
 -DCMAKE_ANDROID_ARCH_ABI=armeabi^
 -DCMAKE_ANDROID_NDK=%GDSTL_NDK%^
 -DCMAKE_ANDROID_STL_TYPE=none^
 -G "Ninja"^
 -DGDSTL_PATH=%GDSTL_PATH%^
 -DGDSTL_TARGET=%GDSTL_TARGET%^
 %GDSTL_PATH%
:: Build library (armeabi)
ninja gdstl
:: Generate build files (armeabi-v7a)
mkdir %GDSTL_PATH%\build\android-armeabi-v7a
cd  %GDSTL_PATH%\build\android-armeabi-v7a
cmake ^
 -DCMAKE_BUILD_TYPE=Release^
 -DCMAKE_SYSTEM_NAME=Android^
 -DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a^
 -DCMAKE_ANDROID_NDK=%GDSTL_NDK%^
 -DCMAKE_ANDROID_STL_TYPE=none^
 -G "Ninja"^
 -DGDSTL_PATH=%GDSTL_PATH%^
 -DGDSTL_TARGET=%GDSTL_TARGET%^
 %GDSTL_PATH%
:: Build library (armeabi-v7a)
ninja gdstl
:: Generate build files (x86)
mkdir %GDSTL_PATH%\build\android-x86
cd  %GDSTL_PATH%\build\android-x86
cmake ^
 -DCMAKE_BUILD_TYPE=Release^
 -DCMAKE_SYSTEM_NAME=Android^
 -DCMAKE_ANDROID_ARCH_ABI=x86^
 -DCMAKE_ANDROID_NDK=%GDSTL_NDK%^
 -DCMAKE_ANDROID_STL_TYPE=none^
 -G "Ninja"^
 -DGDSTL_PATH=%GDSTL_PATH%^
 -DGDSTL_TARGET=%GDSTL_TARGET%^
 %GDSTL_PATH%
:: Build library (armeabi-x86)
ninja gdstl
:: Go back
cd  %GDSTL_PATH%
