:: This is an prebuilt script part of the Plugin SDK
@echo off

cd %~dp0
cmd /C "_stylehelper.bat ..\src cpp"
cmd /C "_stylehelper.bat ..\src h"
cmd /C "_stylehelper.bat ..\src hpp"
cmd /C "_stylehelper.bat ..\inc cpp"
cmd /C "_stylehelper.bat ..\inc h"
cmd /C "_stylehelper.bat ..\inc hpp"
cmd /C "_stylehelper.bat ..\wizard cpp"
cmd /C "_stylehelper.bat ..\wizard h"
cmd /C "_stylehelper.bat ..\wizard hpp"
