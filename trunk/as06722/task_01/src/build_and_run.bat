@echo off
setlocal EnableExtensions
chcp 65001 >nul
set "PYTHONUTF8=1"
set "PYTHONIOENCODING=utf-8"
cd /d "%~dp0"

echo ========================================
echo   [task_01] Build and run
echo ========================================
echo.

where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] CMake was not found in PATH.
    echo Install CMake or add it to PATH, then run this file again.
    goto error
)

where py >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Python launcher "py" was not found.
    echo Install Python for Windows and enable the Python launcher.
    goto error
)

echo [1/3] Configuring CMake...
if exist "build" rmdir /s /q "build"
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
if errorlevel 1 goto error

echo.
echo [2/3] Building project...
cmake --build build --config Release
if errorlevel 1 goto error

set "APP=build\Release\task01.exe"
if not exist "%APP%" set "APP=build\task01.exe"
if not exist "%APP%" (
    echo [ERROR] task01.exe was not created.
    goto error
)

echo.
echo [3/3] Running program...
echo The graph will open automatically after the calculation.
echo.
"%APP%"
if errorlevel 1 goto error

echo.
echo Program finished successfully.
pause
exit /b 0

:error
echo.
echo ========================================
echo Build or program execution failed.
echo Read the error messages above.
echo ========================================
pause
exit /b 1
