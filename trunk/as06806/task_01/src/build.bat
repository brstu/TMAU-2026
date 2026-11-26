@echo off
chcp 65001 >nul
setlocal

cd /d "%~dp0"

echo ========================================
echo       CONFIGURE PROJECT
echo ========================================
cmake -S . -B build
if errorlevel 1 goto error

echo.
echo ========================================
echo       BUILD PROJECT
echo ========================================
cmake --build build --config Release
if errorlevel 1 goto error

echo.
echo ========================================
echo       RUN PROGRAM
echo ========================================
if exist "build\Release\task_01.exe" (
    "build\Release\task_01.exe"
) else if exist "build\task_01.exe" (
    "build\task_01.exe"
) else (
    echo Error: executable not found.
    goto error
)
if errorlevel 1 goto error

echo.
echo ========================================
echo       VISUALISATION
echo ========================================
py -c "import matplotlib" >nul 2>&1
if errorlevel 1 (
    echo Matplotlib not found, installing...
    py -m pip install matplotlib
    if errorlevel 1 goto error
) else (
    echo Matplotlib is already installed.
)
py visualise.py result.csv
if errorlevel 1 goto error

echo.
echo Done.
pause
exit /b 0

:error
echo.
echo Build or run error.
pause
exit /b 1