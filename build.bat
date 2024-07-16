@echo off

set "CC=gcc"
set "TEMPDIR=%CD%\temp"
set "BINDIR=%CD%\binary"
set "BUILDTOOLSDIR=%CD%\buildtools"
set "OUTFILE=webasm.js"
set "LIBDIR="
set "LIBFLAGS="
set "CFLAGS=-mx32"
set "LFLAGS="
REM BFLAGS for both compiler and linker
set "BFLAGS="


if defined  %1 (
    set fun=%1
    shift
    call :%fun% %*
) else call :fastdebug
exit /b %ERRORLEVEL%

:fastdebug
set "BFLAGS=%BFLAGS% -O0 -g"
call :build
exit /b %ERRORLEVEL%

:debug
set "BFLAGS=%BFLAGS% -Og -g"
call :build
exit /b %ERRORLEVEL%

:release
set "BFLAGS=%BFLAGS% -Os"
call :build
exit /b %ERRORLEVEL%


:build
setlocal

set "PATH=%PATH%;%BUILDTOOLSDIR%"
::set "BUILDTOOLS=^"%CD%\BUILDTOOLS.BAT^""
set COMPILE_CMD="%CC% -c ""%%SOURCEDIR%%\%%INFILE%%"" -o ""%TEMPDIR%\%%OUTFILE%%"" %BFLAGS% %CFLAGS% %%BFLAGS%% %%CFLAGS%%"
set LINKER_CMD="%CC% %TEMPDIR%\*.o -o %BINDIR%\%OUTFILE%"
set DEPS_CMD="%CC% -MM ""%%SOURCEDIR%%\%%INFILE%%"""

mkdir "%TEMPDIR%"
mkdir "%BINDIR%"

setlocal
cd source/
call build.bat
endlocal

cmd /c %LINKER_CMD%

endlocal
exit /b 0


:clean
echo deleting "%TEMPDIR%"
rmdir "%TEMPDIR%" /s
echo clearing "%BINDIR%"
rmdir "%BINDIR%" /s
mkdir "%BINDIR%"
exit /b 0