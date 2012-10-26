@echo off

cmd /C "_stylehelper.bat ..\src cpp" 2>NUL
cmd /C "_stylehelper.bat ..\src h" 2>NUL
cmd /C "_stylehelper.bat ..\src hpp" 2>NUL
cmd /C "_stylehelper.bat ..\src c" 2>NUL
cmd /C "_stylehelper.bat ..\src cc" 2>NUL
cmd /C "_stylehelper.bat ..\src inc" 2>NUL

cmd /C "_stylehelper.bat ..\inc cpp" 2>NUL
cmd /C "_stylehelper.bat ..\inc h" 2>NUL
cmd /C "_stylehelper.bat ..\inc hpp" 2>NUL
cmd /C "_stylehelper.bat ..\inc c" 2>NUL
cmd /C "_stylehelper.bat ..\inc cc" 2>NUL
cmd /C "_stylehelper.bat ..\inc inc" 2>NUL

pause