@echo off

:: Полностью удаляем старую папку сборки, чтобы сбросить кэш генератора
if exist build (rmdir /s /q build)
mkdir build
cd build

echo ===================================================
echo [STEP 1/3] Configuring project via CMake...
echo ===================================================
:: Исправлен путь к исходникам (строго две точки без пробелов)
"C:\Program Files\CMake\bin\cmake.exe" .. -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=C:/w64devkit/bin/g++.exe
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    goto end
)

echo.
echo ===================================================
echo [STEP 2/3] Building project modules...
echo ===================================================
"C:\Program Files\CMake\bin\cmake.exe" --build .
if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed.
    goto end
)

echo.
echo ===================================================
echo [STEP 3/3] Running simulation (Variant 19)...
echo ===================================================
ControlModels.exe

echo.
echo ===================================================
echo Generating and opening graph automatically...
echo ===================================================
cd ..
py src/plot.py

:end
echo.
echo ===================================================
echo Process finished. Press any key to exit.
echo ===================================================
pause