@echo off
setlocal

:: Переходим в папку, где лежит сам батник (корень проекта task_01)
cd /d "%~dp0"

:: 1. Принудительно закрываем старые процессы, чтобы папка build не была занята
taskkill /F /IM ControlModels.exe /T >nul 2>&1
taskkill /F /IM mingw32-make.exe /T >nul 2>&1

:: 2. Удаляем старую папку сборки
if exist build (rmdir /s /q build)
mkdir build

:: 3. Создаем папку doc, если её нет (чтобы программа могла сохранить CSV)
if not exist doc (mkdir doc)

:: 3a. (опционально) Чистим старые CSV, чтобы plot.py взял именно новый файл.
::     Раскомментируйте строку ниже, если нужно.
:: del /q doc\*.csv >nul 2>&1

:: 4. Создаем копию make.exe в w64devkit, чтобы CMake не ругался
if not exist "D:\qw\w64devkit\bin\make.exe" (
    copy "D:\qw\w64devkit\bin\mingw32-make.exe" "D:\qw\w64devkit\bin\make.exe" >nul
)

:: 5. Добавляем компиляторы в PATH для текущей сессии
set PATH=D:\qw\w64devkit\bin;%PATH%

echo ===================================================
echo [STEP 1/3] Configuring project via CMake (Ninja)...
echo ===================================================
pushd build
"C:\Program Files\CMake\bin\cmake.exe" .. -G "Ninja" -DCMAKE_C_COMPILER=gcc.exe -DCMAKE_CXX_COMPILER=g++.exe -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    popd
    goto end
)

echo.
echo ===================================================
echo [STEP 2/3] Building project modules...
echo ===================================================
"C:\Program Files\CMake\bin\cmake.exe" --build .
if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed.
    popd
    goto end
)
popd

echo.
echo ===================================================
echo [STEP 3/3] Running simulation (Variant 19)...
echo ===================================================
:: Запускаем ИЗ КОРНЯ проекта, чтобы CSV попали в doc/, а не в build/doc/
build\ControlModels.exe
if %errorlevel% neq 0 (
    echo [ERROR] Simulation failed.
    goto end
)

echo.
echo ===================================================
echo Generating and opening graph automatically...
echo ===================================================
py src\plot.py

:end
echo.
echo ===================================================
echo Process finished. Press any key to exit.
echo ===================================================
pause
endlocal