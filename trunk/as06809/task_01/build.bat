@echo off
chcp 65001 >nul

:: 1. Удаляем старую папку сборки
if exist build (rmdir /s /q build)
mkdir build
cd build

echo ===================================================
echo [STEP 1/3] Configuring project via CMake...
echo ===================================================
:: Указываем точные пути, которые вы нашли: D:\CMake и C:\msys64
:: Добавляем флаг -DCMAKE_MAKE_PROGRAM, чтобы CMake точно знал, где взять make из MSYS64
"D:\CMake\bin\cmake.exe" .. -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe" -DCMAKE_MAKE_PROGRAM="C:/msys64/ucrt64/bin/mingw32-make.exe"

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] CMake configuration failed! Check paths above.
    pause
    goto end
)

echo.
echo ===================================================
echo [STEP 2/3] Building project modules...
echo ===================================================
"D:\CMake\bin\cmake.exe" --build .

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Compilation failed!
    pause
    goto end
)

echo.
echo ===================================================
echo [STEP 3/3] Running simulation...
echo ===================================================
ControlModels.exe

echo.
echo ===================================================
echo Generating and opening graph automatically...
echo ===================================================
cd ..
:: Используем команду python (или py, если у вас работает именно она)
python src/plot.py

:end
echo.
echo ===================================================
echo Process finished. Press any key to exit.
echo ===================================================
pause