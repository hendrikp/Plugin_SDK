:: Set project relevant settings
set VCPROJECT="..\project\PluginManager.vcxproj"
set VCTOOLS="%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"


:: Compile x86
call %VCTOOLS% x86

MSBuild %VCPROJECT% /t:Rebuild /p:Configuration=Release
IF ERRORLEVEL 1 GOTO COMPILERROR


:: Compile x64
call %VCTOOLS% x64

MSBuild %VCPROJECT% /t:Rebuild /p:Configuration=Release
IF ERRORLEVEL 1 GOTO COMPILERROR

:: End
GOTO ENDOK

:COMPILERROR

::Trigger a Syntax error
--ERROR_DETECTED--

:ENDOK
