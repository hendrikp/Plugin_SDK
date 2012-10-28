:: Build all Plugins and the Game.DLL

:: Build all Plugins
set FILES="%~dp0..\..\*"
for /D %%a in (%FILES%) do (

  if exist %%a\tools\build.bat (
    cd %%a\tools\
    call %%a\tools\build.bat
    if ERRORLEVEL 1 goto COMPILE_ERROR
  )
)

:: Build the Game.DLL
cd %~dp0
call build_game.bat
if ERRORLEVEL 1 goto COMPILE_ERROR

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