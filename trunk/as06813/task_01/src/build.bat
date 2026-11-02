@echo off
cd /d "%~dp0"

echo [1/3] Generating project files
cmake -S . -B build
if errorlevel 1 goto fail

echo.
echo [2/3] Compiling
cmake --build build --config Release
if errorlevel 1 goto fail

echo.
echo [3/3] Starting program
echo.
if exist build\Release\TMAU_Lab1.exe (
    build\Release\TMAU_Lab1.exe
) else (
    build\TMAU_Lab1.exe
)
pause
exit /b 0

:fail
echo ERROR: build failed
pause
exit /b 1
