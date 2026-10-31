@echo off
setlocal
title TiMAY - Build and Run

cd /d "%~dp0"

echo ========================================
echo          TiMAY BUILD PROJECT
echo ========================================
echo.

where cmake >nul 2>nul
if errorlevel 1 (
    echo ERROR: CMake was not found in PATH.
    echo Install CMake and restart the terminal.
    pause
    exit /b 1
)

if not exist "CMakeLists.txt" (
    echo ERROR: CMakeLists.txt was not found.
    echo Place run.bat next to CMakeLists.txt.
    pause
    exit /b 1
)

echo Configuring project...
cmake -S "%~dp0." -B "%~dp0build"

if errorlevel 1 goto error

echo.
echo Building project...
cmake --build "%~dp0build" --config Release

if errorlevel 1 goto error

echo.
echo BUILD COMPLETED.
echo.

if exist "%~dp0build\Release\TiMAY.exe" (
    "%~dp0build\Release\TiMAY.exe"
) else if exist "%~dp0build\bin\TiMAY.exe" (
    "%~dp0build\bin\TiMAY.exe"
) else if exist "%~dp0build\TiMAY.exe" (
    "%~dp0build\TiMAY.exe"
) else (
    echo TiMAY.exe was not found in the expected locations.
    echo Check the build output above.
)

echo.
pause
exit /b 0

:error
echo.
echo BUILD FAILED. Read the errors above.
echo.
pause
exit /b 1