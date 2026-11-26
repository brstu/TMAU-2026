@echo off
setlocal EnableExtensions
chcp 65001 >nul
cd /d "%~dp0"

echo =======================================
echo  ЛР3 (as06818): сборка, тесты, Doxygen
echo =======================================

if not exist build mkdir build
cd build

echo [1/4] Конфигурация и сборка проекта...
cmake .. -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 goto :fail
cmake --build . --parallel
if errorlevel 1 goto :fail

echo.
echo [2/4] Запуск модульных тестов (ctest)...
ctest --output-on-failure
if errorlevel 1 goto :fail

echo.
echo [3/4] Генерация Doxygen-документации...
cd /d "%~dp0"
doxygen Doxyfile
if errorlevel 1 goto :fail

echo.
echo [4/4] Готово: сборка, тесты и документация выполнены успешно.
echo       Документация: doc\html\index.html
pause
exit /b 0

:fail
cd /d "%~dp0"
echo.
echo [ОШИБКА] Один из этапов завершился с ошибкой.
pause
exit /b 1
