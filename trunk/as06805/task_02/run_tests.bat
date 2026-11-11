@echo off
rem =====================================================================
rem  Лабораторная работа №2 (задание 2).
rem  Локальная автоматизация: сборка проекта, запуск модульных тестов
rem  (Google Test) и генерация отчёта о покрытии кода (gcovr).
rem
rem  Использование: run_tests.bat
rem =====================================================================
chcp 65001 >nul
cd /d "%~dp0"

set "PROJECT_DIR=%CD%"
set "BUILD_DIR=%PROJECT_DIR%\build"
set "REPORT_DIR=%PROJECT_DIR%\doc"

echo ============================================================
echo  ЛР №2: сборка проекта, модульные тесты и анализ покрытия
echo ============================================================
echo  Каталог проекта: %PROJECT_DIR%
echo.

rem --- 1. Конфигурация и сборка ---
echo [1/4] Конфигурация проекта (CMake)...
cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%"
if errorlevel 1 goto :fail

echo [2/4] Сборка проекта...
cmake --build "%BUILD_DIR%" --config Debug
if errorlevel 1 goto :fail

rem --- 2. Модульные тесты ---
echo.
echo [3/4] Запуск модульных тестов (Google Test + CTest)...
ctest --test-dir "%BUILD_DIR%" -C Debug --output-on-failure
if errorlevel 1 goto :fail

rem --- 3. Анализ покрытия ---
echo.
echo [4/4] Анализ покрытия кода (gcovr)...
call :run_coverage
if errorlevel 1 goto :fail

echo.
echo ============================================================
echo  Готово!
echo  Отчёт о покрытии: %REPORT_DIR%\coverage_report.txt
echo  HTML-отчёт:       %REPORT_DIR%\coverage_report.html
echo ============================================================
pause
exit /b 0

rem =====================================================================
rem  Подпроцедура анализа покрытия кода
rem =====================================================================
:run_coverage
set "WORK_DIR=%TEMP%\tmau_lab2_coverage"
set "WORK_SLASH=%WORK_DIR:\=/%"

rem --- ищем компилятор MinGW: он нужен для инструментированной сборки ---
set "MINGW_BIN="
for /f "delims=" %%P in ('where mingw32-make.exe 2^>nul') do set "MINGW_BIN=%%~dpP"
if not defined MINGW_BIN for /f "delims=" %%P in ('dir /b /ad "%LOCALAPPDATA%\Microsoft\WinGet\Packages\BrechtSanders.WinLibs*" 2^>nul') do if exist "%LOCALAPPDATA%\Microsoft\WinGet\Packages\%%P\mingw64\bin\mingw32-make.exe" set "MINGW_BIN=%LOCALAPPDATA%\Microsoft\WinGet\Packages\%%P\mingw64\bin"
if not defined MINGW_BIN goto :no_mingw
set "PATH=%MINGW_BIN%;%PATH%"

rem --- ищем gcovr ---
python -c "import gcovr" >nul 2>&1
if errorlevel 1 python -m pip install --quiet gcovr >nul 2>&1
python -c "import gcovr" >nul 2>&1
if errorlevel 1 goto :no_gcovr

rem --- рабочий каталог сборки ---
rem  Сборка выполняется во временном каталоге с ASCII-путём: утилиты MinGW
rem  некорректно работают с кириллицей в путях.
echo [INFO] Инструментированная сборка во временном каталоге: %WORK_DIR%
rmdir /s /q "%WORK_DIR%" >nul 2>&1
mkdir "%WORK_DIR%" >nul 2>&1
xcopy "%PROJECT_DIR%\src" "%WORK_DIR%\src" /E /I /Q /Y >nul
xcopy "%PROJECT_DIR%\test" "%WORK_DIR%\test" /E /I /Q /Y >nul
if exist "%PROJECT_DIR%\third_party" xcopy "%PROJECT_DIR%\third_party" "%WORK_DIR%\third_party" /E /I /Q /Y >nul
copy /Y "%PROJECT_DIR%\CMakeLists.txt" "%WORK_DIR%\" >nul

cmake -S "%WORK_DIR%" -B "%WORK_DIR%\build" -G "MinGW Makefiles" -DTMAU_ENABLE_COVERAGE=ON
if errorlevel 1 goto :no_coverage

cmake --build "%WORK_DIR%\build"
if errorlevel 1 goto :no_coverage

ctest --test-dir "%WORK_DIR%\build" --output-on-failure
if errorlevel 1 goto :no_coverage

if not exist "%REPORT_DIR%" mkdir "%REPORT_DIR%" >nul 2>&1

python -m gcovr --root "%WORK_DIR%" --filter "%WORK_SLASH%/src/" --exclude ".*main\.cpp" --exclude-unreachable-branches --print-summary --output "%REPORT_DIR%\coverage_report.txt" "%WORK_DIR%\build"
if errorlevel 1 goto :no_coverage

python -m gcovr --root "%WORK_DIR%" --filter "%WORK_SLASH%/src/" --exclude ".*main\.cpp" --exclude-unreachable-branches --html "%REPORT_DIR%\coverage_report.html" "%WORK_DIR%\build"
if errorlevel 1 goto :no_coverage

echo [OK] Отчёты о покрытии сохранены в %REPORT_DIR%
exit /b 0

:no_mingw
echo [ВНИМАНИЕ] Не найден компилятор MinGW - анализ покрытия пропущен.
echo           Модульные тесты при этом выполнены успешно.
exit /b 0

:no_gcovr
echo [ВНИМАНИЕ] Не найден gcovr - анализ покрытия пропущен.
echo           Установите его командой: python -m pip install gcovr
exit /b 0

:no_coverage
echo [ВНИМАНИЕ] Анализ покрытия не выполнен, смотрите сообщения выше.
exit /b 0

rem =====================================================================
:fail
echo.
echo ============================================================
echo  ОШИБКА: выполнение скрипта прервано. Смотрите сообщения выше.
echo ============================================================
pause
exit /b 1
