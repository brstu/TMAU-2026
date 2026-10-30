@echo off
chcp 65001 > nul
echo ===================================
echo Building Project for Task 01...
echo ===================================

if not exist "build" mkdir build
cd build

cmake ..
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b %ERRORLEVEL%
)

cmake --build .
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo ===================================
echo Build Successful! Running program...
echo ===================================

if exist "src\Debug\task_01.exe" (
    src\Debug\task_01.exe
) else if exist "src\Release\task_01.exe" (
    src\Release\task_01.exe
) else if exist "Debug\task_01.exe" (
    Debug\task_01.exe
) else (
    task_01.exe
)

echo ===================================
echo Generating Plot...
echo ===================================
cd ..
python plot_results.py

pause