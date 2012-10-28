:: Set project relevant settings

:: not buildable without solution 
:: set VCPROJECT="..\..\Game\GameDll\GameDll.vcxproj"

set VCPROJECT="..\..\Solutions\CryEngine_GameCodeOnly.sln"
set VCTOOLS="%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"

:: Compile x86
call %VCTOOLS% x86

MSBuild %VCPROJECT% /t:Game\CryGame:Rebuild /p:Configuration=Release
IF ERRORLEVEL 1 GOTO COMPILERROR


:: Compile x64
call %VCTOOLS% x64

MSBuild %VCPROJECT% /t:Game\CryGame:Rebuild /p:Configuration=Release
IF ERRORLEVEL 1 GOTO COMPILERROR

:: End
GOTO ENDOK

:COMPILERROR

::Trigger a Syntax error
--ERROR_DETECTED--

:ENDOK
