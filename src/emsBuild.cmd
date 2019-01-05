echo off
if "%~1" == "" (
        echo.
        echo Please type: $0 [Debug/Release/RelWithDebInfo/MinSizeRel]
        echo.
        goto end
)

set EMS_DIR=cmake-ems-%~1

set exist=0
if NOT EXIST %EMS_DIR%  set exist=1
if NOT EXIST "%EMS_DIR%\Makefile" set exist=1

if %exist% EQU 1 (
        echo. 
        echo Have you generate Makefile?
        echo     please run: emsCMakeGen.cmd
        echo.
) else (
    cd %EMS_DIR%
    echo.
    echo. Start mingw32-make... 
    echo.
    mingw32-make
    echo.
    echo.
    cd ..
)

:end