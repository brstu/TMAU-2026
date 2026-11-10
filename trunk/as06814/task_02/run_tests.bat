@echo off
chcp 65001 > nul
set "MSYS_BIN=C:\saf\MSYS2\ucrt64\bin"
set "PYTHON_SCRIPTS=C:\Users\liosh\AppData\Local\Python\pythoncore-3.14-64\Scripts"
set "PATH=%MSYS_BIN%;%PYTHON_SCRIPTS%;%PATH%"

echo ========================================================
echo  Компиляция проекта и запуск Google Test (ЛР №2)
echo ========================================================

if exist build rmdir /s /q build
mkdir build
cd build

echo ===== Конфигурация CMake =====
cmake -G "MinGW Makefiles" ^
  -DCMAKE_CXX_COMPILER="%MSYS_BIN%/g++.exe" ^
  -DCMAKE_MAKE_PROGRAM="%MSYS_BIN%/mingw32-make.exe" ..
if %errorlevel% neq 0 (
    echo [ОШИБКА] Ошибка конфигурации CMake.
    pause
    exit /b 1
)

echo ===== Сборка тестов =====
cmake --build .
if %errorlevel% neq 0 (
    echo [ОШИБКА] Ошибка сборки проекта.
    pause
    exit /b 1
)

echo.
echo ========================================================
echo  Запуск модульных тестов (Google Test / CTest)...
echo ========================================================
ctest --output-on-failure
if %errorlevel% neq 0 (
    echo [ОШИБКА] Часть тестов не пройдена!
    pause
    exit /b 1
)

echo.
echo ========================================================
echo  Генерация отчета о покрытии кода (gcovr)...
echo ========================================================
where gcovr >nul 2>nul
if %errorlevel% equ 0 (
    gcovr -r .. --filter "../src/.*" --txt coverage_report.txt
    gcovr -r .. --filter "../src/.*"
    echo Отчет сохранен в build/coverage_report.txt
) else (
    echo [ИНФО] gcovr не найден в PATH. Отчет покрытия пропущен.
)

cd ..
echo ========================================================
echo  Тестирование завершено успешно!
echo ========================================================
pause