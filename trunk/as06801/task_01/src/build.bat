@echo off
setlocal
cd /d "%~dp0"

echo ==================================================
echo  Lab 1: build with CMake
echo ==================================================

echo.
echo [1/2] Configure project (cmake)...
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto error

echo.
echo [2/2] Build (cmake --build)...
cmake --build build --config Release
if errorlevel 1 goto error

echo.
echo ==================================================
echo  Run program (interactive)
echo ==================================================
if exist build\bin\sim.exe (set EXE=build\bin\sim.exe) else (set EXE=build\bin\Release\sim.exe)
"%EXE%"
if errorlevel 1 goto error

echo.
echo Done. Plots are shown right after the simulation.
echo Non-interactive example: %EXE% --model 1.3 --input all --n 30
pause
exit /b 0

:error
echo.
echo ERROR: build or run failed.
pause
exit /b 1
