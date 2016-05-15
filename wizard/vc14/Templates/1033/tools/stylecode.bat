@echo OFF

set "ASTYLE=%~dp0AStyle\AStyle.exe"
set "APROFILE=%~dp0codestyle.astylerc"
set "SOURCEDIR=%~dp0.."

:: sourcecode path to indent
if not ("%~1")==("") (
  set "SOURCEDIR=%~1"
)

:: use a personal style or a style suitable for source control / team?
if not ("%~2")==("") (
  if exist "%~2" (
    set "APROFILE=%~2"
  )
)

:: Now indent the files

set EXTENSIONS=h hpp c cc cpp

echo Indenting the files '%EXTENSIONS%' in '%SOURCEDIR%' with profile '%APROFILE%'

for %%i in (%EXTENSIONS%) do (
  "%ASTYLE%" --options="%APROFILE%" -RnqZ "%SOURCEDIR%\src\*.%%i" 2> nul
  "%ASTYLE%" --options="%APROFILE%" -RnqZ "%SOURCEDIR%\inc\*.%%i" 2> nul
)

exit /B 0