echo off
echo.
echo.

set argC=0
for %%x in (%*) do Set /A argC+=1

if "%EMSCRIPTEN%" == "" (
    if %argC% NEQ 2 (
        goto message
    ) else (
        goto cont1
    )
)

if %argC% LEQ 1 (
    if "%~1" == "" (
        set BUILD_TYPE=MinSizeRel
    ) else (
        set "BUILD_TYPE=%~1"
    )
    set EMS_DIR=cmake-ems-%~1
    goto cont2
)

if %argC% EQU 2 (
        goto cont1
)


:message
        echo.
        echo Please set EMSCRIPTEN environment var, or pass location via command line
        echo     for example: 
        echo            %~0 C:\emsdk\emscripten\1.38.8 Debug/Release/RelWithDebInfo/MinSizeRel
        echo        or  %~0 Debug/Release/RelWithDebInfo/MinSizeRel
        echo        or  %~0 
        echo              "MinSizeRel" is default building
        echo              "emsOut" is default directory
        echo.
    goto fine


:cont1
        echo "cont 1"
        set "EMSCRIPTEN=%~1"
        if "%~2" == "" (
            set BUILD_TYPE=MinSizeRel
        ) else (
            set "BUILD_TYPE=%~2"
        )
        set EMS_DIR=cmake-ems-%~2

:cont2
        echo "cont 2"

if NOT EXIST %EMS_DIR% (
    mkdir %EMS_DIR%
)

cd %EMS_DIR%

echo.
echo. Start cmake...
echo.
cmake -DCMAKE_TOOLCHAIN_FILE:STRING=%EMSCRIPTEN%\cmake\Modules\Platform\Emscripten.cmake -DCMAKE_BUILD_TYPE:STRING=%BUILD_TYPE% -G "MinGW Makefiles" ..
echo.
echo.

echo.
echo. Start mingw32-make... 
echo.
mingw32-make
echo.
echo.

:end

cd ..

:fine