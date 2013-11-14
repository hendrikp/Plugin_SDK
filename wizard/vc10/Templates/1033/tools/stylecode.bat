@echo OFF

set ASTYLE=%~dp0AStyle\AStyle.exe
set APROFILE=%~dp0codestyle.astylerc

:: Use a personal style or a style suitable for source control / team?
if not (%~1)==() (
  set APROFILE=%~1
)

:: Now indent the files

set EXTENSIONS=h hpp c cc cpp

for %%i in (%EXTENSIONS%) do (
  "%ASTYLE%" --options="%APROFILE%" -RnqZ %~dp0..\src\*.%%i 2> nul
  "%ASTYLE%" --options="%APROFILE%" -RnqZ %~dp0..\inc\*.%%i 2> nul
)

exit /B 0