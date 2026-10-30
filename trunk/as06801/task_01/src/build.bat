@echo off
setlocal
cd /d "%~dp0"

echo ==================================================
echo  Lab 1: build with CMake
echo ==================================================

echo.
echo [1/3] Configure project (cmake)...
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto error

echo.
echo [2/3] Build (cmake --build)...
cmake --build build --config Release
if errorlevel 1 goto error

echo.
echo [3/3] Run program...
echo ==================================================
if exist build\bin\sim.exe (set EXE=build\bin\sim.exe) else (set EXE=build\bin\Release\sim.exe)
"%EXE%" --model 1.3 --input all --n 20
if errorlevel 1 goto error
"%EXE%" --model 2.7 --input all --n 20
if errorlevel 1 goto error
"%EXE%" --model 3.1 --input all --n 40 --y0 1
if errorlevel 1 goto error

echo.
echo Build and run finished successfully.
echo Plots:      python plot.py output
echo All models: %EXE% --model all --input all --n 50 --quiet
pause
exit /b 0

:error
echo.
echo ERROR: build or run failed.
pause
exit /b 1
