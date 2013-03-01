:: This is an built script part of the Plugin SDK
::@echo off

setlocal

set PSDKTOOLDIR=%~dp0

:: Go into Backup directory
cd %~dp0
mkdir SED_Backups
cd SED_Backups

:: Determine current year
set NEWYEAR=
for /f "skip=1 delims=" %%A in ('wmic os get localdatetime') do (
  if not defined NEWYEAR (
    set NEWYEAR=%%A
  )
)
set NEWYEAR=%NEWYEAR:~0,4%

:: Determine CDK Version
set NEWCDKVERSION=
for /f "tokens=1-3 delims=, " %%A in ('%PSDKTOOLDIR%\sigcheck -n -q ""%PSDKTOOLDIR%\..\..\..\Bin32\CrySystem.dll""') do (
  set NEWCDKVERSION=%%~A.%%~B.%%~C
)

:: Set versions
for /f "tokens=1-2 delims=	" %%A in (%PSDKTOOLDIR%\versions.txt) do (
  call "%PSDKTOOLDIR%\setversion.bat" "%%B" "%%A"
)

:: Cleanup Backups
echo Cleaning Temp Files...
cd ..
rmdir /S /Q SED_Backups

endlocal

echo Done
pause