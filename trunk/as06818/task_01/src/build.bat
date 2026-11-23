@echo off
echo Сборка проекта...


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
echo запуск симуляции...
echo ========================================

if exist Release\task_01_as06818.exe (
    Release\task_01_as06818.exe
) else if exist task_01_as06818.exe (
    task_01_as06818.exe
) else (
    echo [ERROR] Executable not found!
)

cd ..
pause