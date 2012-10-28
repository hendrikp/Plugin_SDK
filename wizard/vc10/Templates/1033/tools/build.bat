@echo off

:: Set project relevant settings
set VCPROJECT="..\project\[!output PROJECT_NAME_SAFE].vcxproj"
set VCTOOLS="%VS100COMNTOOLS%..\..\VC\vcvarsall.bat"

IF EXIST %VCTOOLS% (
  :: Compile x86
  call %VCTOOLS% x86

  MSBuild %VCPROJECT% /t:Rebuild /p:Configuration=Release
  IF ERRORLEVEL 1 GOTO COMPILERROR


  :: Compile x64
  call %VCTOOLS% x64

  MSBuild %VCPROJECT% /t:Rebuild /p:Configuration=Release
  IF ERRORLEVEL 1 GOTO COMPILERROR
)

:: End
GOTO ENDOK

:COMPILERROR

::Trigger a Syntax error
--ERROR_DETECTED--

:ENDOK
