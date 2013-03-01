:: This is an built script part of the Plugin SDK
@echo off

setlocal

set PLUGINNAME=%~1
set PLUGINDIR=%PSDKTOOLDIR%\..\..\%PLUGINNAME%\tools
set NEWVERSION=%~2

:: Remove clutter from path
for /f "delims=" %%A in ("%PLUGINDIR%") do (
  set PLUGINDIR=%%~fA
)

:: Convert to comma seperated version
set NEWVERSION_COMMA=
for /f "tokens=1-4 delims=." %%A in ("%NEWVERSION%") do (
  set NEWVERSION_COMMA=%%~A,%%~B,%%~C,%%~D
)

:: Output some progress
echo ---------------------------------------------
echo Set %PLUGINNAME% Version to %NEWVERSION% (c %NEWYEAR%) for %NEWCDKVERSION%
echo %PLUGINDIR%

:: Definitions for sed usage
set WRITEOPT=-i
set SEDCMD= %PSDKTOOLDIR%\sed

:: Replace Header Version
%SEDCMD% %WRITEOPT% -n -r "1h;1!H;${g;s/(const\s*char\*\s*GetVersion\s*\(\s*\)\s*const\s*\{\s*return\s*\")[0-9\.]*(\"\;\s*\}\s*\;)/\1%NEWVERSION%\2/p}" "%PLUGINDIR%\..\src\CPlugin*.h"

:: Remove Unicode
type "%PLUGINDIR%\..\project\version.rc" > "%PLUGINDIR%\..\project\_version.rc"
del "%PLUGINDIR%\..\project\version.rc"
ren "%PLUGINDIR%\..\project\_version.rc" "version.rc"

:: Replace Resource Version
%SEDCMD% %WRITEOPT% -r "s/(\s*FILEVERSION\s*).*/\1%NEWVERSION_COMMA%/g" "%PLUGINDIR%\..\project\version.rc"
%SEDCMD% %WRITEOPT% -r "s/(\s*PRODUCTVERSION\s*).*/\1%NEWVERSION_COMMA%/g" "%PLUGINDIR%\..\project\version.rc"

%SEDCMD% %WRITEOPT% -r "s/(\s*VALUE\s*\"FileVersion\"\s*,\s*\").*(\")/\1%NEWVERSION%\2/g" "%PLUGINDIR%\..\project\version.rc"
%SEDCMD% %WRITEOPT% -r "s/(\s*VALUE\s*\"ProductVersion\"\s*,\s*\").*(\")/\1%NEWVERSION%\2/g" "%PLUGINDIR%\..\project\version.rc"

:: Change Copyright Year
%SEDCMD% %WRITEOPT% -r "s/(\s*Copyright\s*\(c\)\s*)[0-9]{4}/\1%NEWYEAR%/g" "%PLUGINDIR%\..\project\version.rc"

:: In License only replace first occurence (http://www.linuxtopia.org/online_books/linux_tool_guides/the_sed_faq/sedfaq4_004.html)
%SEDCMD% %WRITEOPT% -r "0,/(\s*Copyright\s*\(c\)\s*)[0-9]{4}/s//\1%NEWYEAR%/" "%PLUGINDIR%\..\license.txt"

%SEDCMD% %WRITEOPT% -r "s/(\s*Copyright\s*\(c\)\s*)[0-9]{4}/\1%NEWYEAR%/g" "%PLUGINDIR%\*_Installer.nsi"
%SEDCMD% %WRITEOPT% -r "s/(\s*Copyright\s*\(c\)\s*)[0-9]{4}/\1%NEWYEAR%/g" "%PLUGINDIR%\*_WebInstaller.nsi"

:: Replace Installer Version
%SEDCMD% %WRITEOPT% -r "s/(\s*!define\s*VERSION\s*\").*(\")/\1%NEWVERSION%\2/g" "%PLUGINDIR%\*_Installer.nsi"
%SEDCMD% %WRITEOPT% -r "s/(\s*!define\s*VERSION\s*\").*(\")/\1%NEWVERSION%\2/g" "%PLUGINDIR%\*_WebInstaller.nsi"

:: Replace Installer CDK Version
%SEDCMD% %WRITEOPT% -r "s/(\s*!define\s*VERSIONCDK\s*\").*(\")/\1%NEWCDKVERSION%\2/g" "%PLUGINDIR%\*_Installer.nsi"
%SEDCMD% %WRITEOPT% -r "s/(\s*!define\s*VERSIONCDK\s*\").*(\")/\1%NEWCDKVERSION%\2/g" "%PLUGINDIR%\*_WebInstaller.nsi"

:: Replace Latest Version
%SEDCMD% %WRITEOPT% -r "s/(_)[0-9\.]*(\.exe)/\1%NEWVERSION%\2/g" "%PLUGINDIR%\..\latestredist.info"



endlocal