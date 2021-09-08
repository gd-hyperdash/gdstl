@echo off
set GDSTL_TARGET=windows
:: Init vars
call .\scripts\vars.bat
:: Create new folder
if exist %GDSTL_PATH%\build\windows\ (
    rmdir /s /q %GDSTL_PATH%\build\windows
)
mkdir %GDSTL_PATH%\build\windows
cd %GDSTL_PATH%\build\windows
:: Generate build files
cmake ^
 -DCMAKE_BUILD_TYPE=Release^
 -G "Ninja"^
 -DGDSTL_PATH=%GDSTL_PATH%^
 -DGDSTL_TARGET=%GDSTL_TARGET%^
 %GDSTL_PATH%
:: Build library
ninja gdstl
:: Go back
cd %GDSTL_PATH%