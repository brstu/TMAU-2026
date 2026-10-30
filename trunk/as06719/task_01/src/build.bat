
@echo off
chcp 65001 > nul
cd /d "%~dp0"

echo === Настройка проекта ===
if not exist build mkdir build

cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe"

if errorlevel 1 (
    echo Ошибка настройки CMake.
    pause
    exit /b 1
)

echo.
echo === Сборка проекта ===
cmake --build build

if errorlevel 1 (
    echo Ошибка сборки проекта.
    pause
    exit /b 1
)

echo.
echo === Запуск программы ===
build\task_01.exe

echo.
pause