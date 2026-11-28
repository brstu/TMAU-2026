@echo off
chcp 65001 > nul
set "MSYS_BIN=C:\saf\MSYS2\ucrt64\bin"
set "PYTHON_SCRIPTS=C:\Users\liosh\AppData\Local\Python\pythoncore-3.14-64\Scripts"
set "PATH=%MSYS_BIN%;%PYTHON_SCRIPTS%;%PATH%"

echo ========================================================
echo   Сборка и тестирование проекта (ЛР №2)
echo ========================================================

if exist build rmdir /s /q build
mkdir build
cd build

echo [1/3] Настройка и сборка проекта...
cmake -G "MinGW Makefiles" ^
  -DCMAKE_CXX_COMPILER="%MSYS_BIN%/g++.exe" ^
  -DCMAKE_MAKE_PROGRAM="%MSYS_BIN%/mingw32-make.exe" ..\src > build_log.txt 2>&1
if %errorlevel% neq 0 (
    type build_log.txt
    echo [ОШИБКА] Ошибка конфигурации CMake.
    pause
    exit /b 1
)

cmake --build . >> build_log.txt 2>&1
if %errorlevel% neq 0 (
    type build_log.txt
    echo [ОШИБКА] Ошибка сборки проекта.
    pause
    exit /b 1
)

echo.
echo [2/3] Пройденные тесты:
echo --------------------------------------------------------
.\unit_tests.exe > test_out.txt
if %errorlevel% neq 0 (
    type test_out.txt
    echo [ОШИБКА] Часть тестов не пройдена!
    pause
    exit /b 1
)
findstr /C:"[       OK ]" /C:"[==========]" /C:"[  PASSED  ]" test_out.txt
echo --------------------------------------------------------

echo.
echo [3/3] Покрытие кода (src/):
where gcovr >nul 2>nul
if %errorlevel% equ 0 (
    gcovr -r .. --filter ".*/src/.*" --exclude ".*gtest_local.*" --exclude ".*/src/main\.cpp" --txt coverage_report.txt
    gcovr -r .. --filter ".*/src/.*" --exclude ".*gtest_local.*" --exclude ".*/src/main\.cpp"
) else (
    echo [ИНФО] gcovr не найден в PATH.
)

cd ..
echo ========================================================
echo   Готово! Все 19 тестов успешно пройдены.
echo ========================================================
pause