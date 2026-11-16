@echo off
setlocal EnableExtensions
chcp 65001 >nul
cd /d "%~dp0"

echo ==========================================================
echo  ЛР2 (as06818): сборка, Google Test, покрытие кода
echo ==========================================================

REM Каталог сборки: пересоздаём, чтобы конфигурация была чистой.
if exist build rmdir /s /q build
mkdir build
cd build

REM Покрытие (gcov/gcovr) доступно только с GCC-инструментарием.
set "COVERAGE=OFF"
set "GENERATOR="
where g++ >nul 2>nul
if not errorlevel 1 (
    where mingw32-make >nul 2>nul
    if not errorlevel 1 (
        set "GENERATOR=-G "MinGW Makefiles""
        set "COVERAGE=ON"
    )
)

echo [1/3] Конфигурация CMake (покрытие: %COVERAGE%)...
cmake .. %GENERATOR% -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=%COVERAGE%
if errorlevel 1 goto :fail

echo.
echo [2/3] Компиляция проекта и тестов...
cmake --build . --parallel
if errorlevel 1 goto :fail

echo.
echo [3/3] Запуск модульных тестов (ctest)...
ctest --output-on-failure
if errorlevel 1 goto :fail

REM Отчёт о покрытии gcovr (если установлен).
where gcovr >nul 2>nul
if errorlevel 1 (
    echo.
    echo [i] gcovr не найден в PATH - отчёт о покрытии пропущен.
    echo     Установка: python -m pip install gcovr
    goto :done
)

echo.
echo Отчёт о покрытии (gcovr)...
gcovr -r .. --html coverage.html --html-details
if errorlevel 1 (
    echo [!] Не удалось сформировать отчёт покрытия.
    goto :done
)
echo Отчёт покрытия сохранён: build\coverage.html

:done
cd /d "%~dp0"
echo.
echo Готово: тесты выполнены успешно.
pause
exit /b 0

:fail
cd /d "%~dp0"
echo.
echo [ОШИБКА] Сборка или тесты завершились с ошибкой.
pause
exit /b 1
