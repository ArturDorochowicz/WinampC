@echo off
setlocal

set GCC_PATH=C:\Progra~1\MinGW

if "%1"=="debug" (goto debug) else (goto release)

:release
"%GCC_PATH%\bin\mingw32-make.exe" "GCCPATH=%GCC_PATH%" %1 %2 %3 %4 %5 %6 %7 %8 %9
goto end

:debug
"%GCC_PATH%\bin\mingw32-make.exe" "GCCPATH=%GCC_PATH%" "DEBUG=1" %2 %3 %4 %5 %6 %7 %8 %9
goto end

:end

:cleaning
if EXIST WinampC.o (del WinampC.o)
if EXIST Resources.o (del Resources.o)

set GCC_PATH=
endlocal
