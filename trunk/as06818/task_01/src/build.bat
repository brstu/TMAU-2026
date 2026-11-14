@echo off
echo билд...


if not exist build mkdir build
cd build

cmake ..
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b %ERRORLEVEL%
)

cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo ========================================
echo запцск симуляции...
echo ========================================

if exist Release\task_01.exe (
    Release\task_01.exe
) else if exist task_01.exe (
    task_01.exe
) else (
    echo [ERROR] Executable not found!
)

cd ..
pause