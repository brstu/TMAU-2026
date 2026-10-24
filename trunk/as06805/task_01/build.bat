@echo off
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

Debug\task_01.exe || task_01.exe

cd ..
pause