@echo off
chcp 65001 > nul
set "MSYS_BIN=C:\saf\MSYS2\ucrt64\bin"
set "PATH=%MSYS_BIN%;%PATH%"

if exist build rmdir /s /q build
mkdir build
cd build

echo ===== Настройка CMake =====
cmake -G "MinGW Makefiles" ^
  -DCMAKE_CXX_COMPILER="%MSYS_BIN%/g++.exe" ^
  -DCMAKE_MAKE_PROGRAM="%MSYS_BIN%/mingw32-make.exe" ..
if %errorlevel% neq 0 (
    echo [ERROR] Ошибка конфигурации CMake.
    pause
    exit /b 1
)

echo ===== Компиляция =====
cmake --build .

echo ===== Запуск симуляции C++ =====
if exist task_01.exe (
    task_01.exe
) else if exist Release\task_01.exe (
    Release\task_01.exe
) else (
    echo [ERROR] Исполняемый файл не найден.
    pause
    exit /b 1
)

echo.
echo ===== Отрисовка графиков Python =====
cd ..
"C:\Users\liosh\AppData\Local\Python\pythoncore-3.14-64\python.exe" plot.py
if %errorlevel% neq 0 (
    python3 plot.py
)

pause