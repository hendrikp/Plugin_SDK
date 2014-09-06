:: Set project relevant settings

set "VCPROJECT=%~dp0..\..\Solutions\CryEngine_GameCodeOnly.sln"
set "VCTOOLS=%VS110COMNTOOLS%..\..\VC\vcvarsall.bat"

set BUILDCONFIG=Profile
set BUILDPROJECT=Game\CryGame
set BUILDACTION=Rebuild

:: new sdk version is using different project name

if exist "%~dp0..\..\GameSDK" (
  set BUILDPROJECT=Game\CryGameSDK
)

:: now build the game.dll

if exist %VCTOOLS% (
  :: Compile x86
  call "%VCTOOLS%" x86

  MSBuild "%VCPROJECT%" /t:%BUILDPROJECT%:%BUILDACTION% /p:Configuration=%BUILDCONFIG%
  IF ERRORLEVEL 1 GOTO COMPILERROR


  :: Compile x64
  call "%VCTOOLS%" x64

  MSBuild "%VCPROJECT%" /t:%BUILDPROJECT%:%BUILDACTION% /p:Configuration=%BUILDCONFIG%
  IF ERRORLEVEL 1 GOTO COMPILERROR
)


:: End
GOTO ENDOK

:COMPILERROR

::Trigger a Syntax error
--ERROR_DETECTED--

:ENDOK
