@echo off

:: Detection code from https://github.com/SublimeText/NSIS

set nsis_compiler=

if defined NSIS_HOME (
    if exist "%NSIS_HOME%\makensis.exe" (
        set "nsis_compiler=%NSIS_HOME%"
    )
)

if %PROCESSOR_ARCHITECTURE%==x86 (
    Set RegQry=HKLM\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\NSIS
) else (
    Set RegQry=HKLM\SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\NSIS
)

if not defined nsis_compiler (
    for /F "tokens=2*" %%a in ('reg query %RegQry% /v InstallLocation ^|findstr InstallLocation') do set nsis_compiler=%%b
)

if not defined nsis_compiler (
    for %%X in (makensis.exe) do (set nsis_compiler=%%~dp$PATH:X)
)

if defined nsis_compiler (
    "%nsis_compiler%\makensis.exe" "[!output PROJECT_NAME_SAFE]_Installer.nsi"
) else (
    echo "Error, build system cannot find NSIS! Please reinstall it, add makensis.exe to your PATH, or define the NSIS_HOME environment variable."
)
