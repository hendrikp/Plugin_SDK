@echo off

:: Build all Plugins and the Game.DLL
setlocal

:: Determine Version
cd %~dp0
call determine_cdkversion.bat

:: Set start time http://stackoverflow.com/questions/739606/how-long-a-batch-file-takes-to-execute
set t0=%time: =0%

:: Build the Game.DLL (needs to be built first since it is included in Plugin_SDK installer)
setlocal

cd %~dp0
call build_game.bat
if ERRORLEVEL 1 (
  endlocal
  goto COMPILE_ERROR
)

endlocal

:: Build all Plugins
set FILES="%~dp0..\..\*"
for /D %%a in (%FILES%) do (

  if exist %%a\tools\build.bat (
    setlocal

    cd %%a\tools\
    call %%a\tools\build.bat

    if ERRORLEVEL 1 (
      endlocal
      goto COMPILE_ERROR
    )

    endlocal
  )

  if exist %%a\tools\build_installer.bat (
    setlocal

    cd %%a\tools\
    call %%a\tools\build_installer.bat

    if ERRORLEVEL 1 (
      endlocal
      goto COMPILE_ERROR
    )

    endlocal
  )
)

:NO_ERRORS
color A
echo.
echo !!!!!!!!!!!!!!!!!!!!!!!
echo !!!  Build success  !!!
echo !!!!!!!!!!!!!!!!!!!!!!!
echo.
goto ENDOK

:COMPILE_ERROR
color C
echo.
echo !!!!!!!!!!!!!!!!!!!!!!!
echo !!!  Build aborted  !!!
echo !!! Errors detected !!!
echo !!!!!!!!!!!!!!!!!!!!!!!
echo.

:ENDOK
cd %~dp0

:: Save End Time
set t=%time: =0%

:: make t0 into a scaler in 100ths of a second, being careful not 
:: to let SET/A misinterpret 08 and 09 as octal
set /a h=1%t0:~0,2%-100
set /a m=1%t0:~3,2%-100
set /a s=1%t0:~6,2%-100
set /a c=1%t0:~9,2%-100
set /a starttime = %h% * 360000 + %m% * 6000 + 100 * %s% + %c%

:: make t into a scaler in 100ths of a second
set /a h=1%t:~0,2%-100
set /a m=1%t:~3,2%-100
set /a s=1%t:~6,2%-100
set /a c=1%t:~9,2%-100
set /a endtime = %h% * 360000 + %m% * 6000 + 100 * %s% + %c%

:: runtime in 100ths is now just end - start
set /a runtime = %endtime% - %starttime%
set runtime = %s%.%c%

echo Started at %t0% Ended at %t%
echo Ran for %runtime%0 ms

endlocal

pause