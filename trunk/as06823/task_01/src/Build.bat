
@echo off
setlocal
cd /d "%~dp0"
chcp 65001 >nul

echo ==============================================
echo          Сборка и запуск программы
echo ==============================================

REM Если CMakeLists.txt находится в src, переходим в src.
if not exist "CMakeLists.txt" (
    if exist "src\CMakeLists.txt" (
        cd /d "%~dp0src"
    ) else (
        echo ОШИБКА: CMakeLists.txt не найден.
        pause
        exit /b 1
    )
)

echo.
echo [1/4] Проверка CMake...

where cmake >nul 2>nul
if errorlevel 1 (
    echo ОШИБКА: CMake не найден в PATH.
    pause
    exit /b 1
)

echo.
echo [2/4] Настройка и сборка...

cmake -S . -B build
if errorlevel 1 (
    echo ОШИБКА при настройке проекта.
    pause
    exit /b 1
)

cmake --build build --config Release
if errorlevel 1 (
    echo ОШИБКА при сборке программы.
    pause
    exit /b 1
)

if exist "build\Release\lab1.exe" (
    set "APP=build\Release\lab1.exe"
) else if exist "build\lab1.exe" (
    set "APP=build\lab1.exe"
) else (
    echo ОШИБКА: lab1.exe не найден.
    pause
    exit /b 1
)

echo.
echo [3/4] Запуск симуляции...
echo.

"%APP%"
if errorlevel 1 (
    echo ОШИБКА при выполнении программы.
    pause
    exit /b 1
)

echo.
echo [4/4] Проверка Python и Matplotlib...

set "PYTHON_CMD="

where python >nul 2>nul
if not errorlevel 1 (
    python --version >nul 2>nul
    if not errorlevel 1 set "PYTHON_CMD=python"
)

if not defined PYTHON_CMD (
    where py >nul 2>nul
    if not errorlevel 1 (
        py -3 --version >nul 2>nul
        if not errorlevel 1 set "PYTHON_CMD=py -3"
    )
)

if not defined PYTHON_CMD (
    echo Python не найден.
    echo Установите Python 3 и повторите запуск.
    echo Результаты симуляции сохранены в results.csv.
    pause
    exit /b 0
)

%PYTHON_CMD% -c "import matplotlib" >nul 2>nul

if errorlevel 1 (
    echo Matplotlib не найден. Устанавливаем...

    %PYTHON_CMD% -m pip install --user matplotlib

    if errorlevel 1 (
        echo ОШИБКА установки Matplotlib.
        echo Попробуйте команду:
        echo python -m pip install matplotlib
        pause
        exit /b 1
    )
)

if exist "charts.py" (
    set "CHART_SCRIPT=charts.py"
) else if exist "src\charts.py" (
    set "CHART_SCRIPT=src\charts.py"
) else (
    echo ОШИБКА: charts.py не найден.
    pause
    exit /b 1
)

echo.
echo Построение графиков...

%PYTHON_CMD% "%CHART_SCRIPT%" results.csv

if errorlevel 1 (
    echo ОШИБКА при построении графиков.
    pause
    exit /b 1
)

echo.
echo Программа завершена.
pause
endlocal
