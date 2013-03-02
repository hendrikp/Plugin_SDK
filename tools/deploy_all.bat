:: This is an built script part of the Plugin SDK
@echo off

setlocal

set PSDKTOOLDIR=%~dp0
cd %~dp0

call %PSDKTOOLDIR%\setversion_all.bat
call %PSDKTOOLDIR%\build_all.bat
call %PSDKTOOLDIR%\upload_all.bat

endlocal