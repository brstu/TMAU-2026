@echo off
echo ==========================================
echo Building Model Simulation Project...
echo ==========================================
if not exist build mkdir build
cd build
cmake ..
if %errorlevel% neq 0 (
    echo.
    echo ERROR: CMake configuration failed!
    pause
    exit /b
)

cmake --build . --config Release
if %errorlevel% neq 0 (
    echo.
    echo ERROR: Compilation failed! Check the errors above.
    pause
    exit /b
)

echo.
echo ==========================================
echo Build finished successfully!
echo Executable is located at: build\Release\tmau_task_01.exe
echo ==========================================
pause