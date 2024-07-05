@setlocal
@echo off

REM this file is intended for another batch file to call it
if not defined COMPILE_CMD(
    echo Error: %%COMPILE_CMD%% not defined
    endlocal && exit /b 1
)

set "CFLAGS="
set "COMPLIST="
setlocal && cd shaders/ && format_shaders.py && endlocal
setlocal && cd types/   && call build.bat && endlocal
setlocal && cd objects/ && call build.bat && endlocal
call easybuild "%COMPLIST%"

endlocal