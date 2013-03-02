:: This is an built script part of the Plugin SDK
@echo off

setlocal

set PSDKTOOLDIR=%~dp0

set CURLCMD=%PSDKTOOLDIR%\curl\curl

:: Get User Account ( not included in repository; use your own account ;) )
set BINTRAYAPI=
set BINTRAYUSR=
for /f "tokens=1-2 delims=	" %%A in (%PSDKTOOLDIR%\bintray.tmp) do (
  echo -
  echo User %%A API %%B
  set BINTRAYAPI=%%B
  set BINTRAYUSR=%%A
)

:: Upload to Bintray
for /f "tokens=1-2 delims=	" %%A in (%PSDKTOOLDIR%\versions.txt) do (
  echo -
  echo Uploading %%B %%A
  %CURLCMD% --verbose --progress-bar -k -T "%PSDKTOOLDIR%\..\..\%%B\tools\%%B_%%A.exe" -u%BINTRAYUSR%:%BINTRAYAPI% -H "X-Bintray-Package:%%B" -H "X-Bintray-Version:%%A" https://api.bintray.com/content/%BINTRAYUSR%/CryEngine-Plugins/%%B_%%A.exe
)

endlocal

echo Upload  Done
pause