@echo off
:: Build all Plugins and the Game.DLL
setlocal

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
)

:: Build the Game.DLL
setlocal

cd %~dp0
call build_game.bat
if ERRORLEVEL 1 (
  endlocal
  goto COMPILE_ERROR
)

endlocal

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
pause