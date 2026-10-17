@echo off

:: Create build directory if it doesn't exist and navigate into it
if not exist build (mkdir build)
cd build

echo ===================================================
echo [STEP 1/3] Configuring project via CMake...
echo ===================================================
cmake .. 
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed. Ensure CMake and compiler are installed.
    goto end
)

echo.
echo ===================================================
echo [STEP 2/3] Building project modules...
echo ===================================================
cmake --build .
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
echo Generating graphs automatically via Python...
echo ===================================================
:: Возвращаемся в корень проекта, чтобы правильно вызвать скрипт
cd ..
py src/plot.py

:end
echo.
echo ===================================================
echo Process finished. Press any key to exit.
echo ===================================================
pause
