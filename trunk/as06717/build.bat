@echo off
chcp 65001 > nul

if exist build (
    rmdir /q /s build
)

echo [1/2] Configuring project...
cmake -B build -S .

echo [2/2] Building project...
cmake --build build --config Release

set "EXE_PATH="
if exist "build\src\Release\LabWork1.exe" (
    set "EXE_PATH=build\src\Release\LabWork1.exe"
) else if exist "build\src\Debug\LabWork1.exe" (
    set "EXE_PATH=build\src\Debug\LabWork1.exe"
) else if exist "build\Release\LabWork1.exe" (
    set "EXE_PATH=build\Release\LabWork1.exe"
) else if exist "build\Debug\LabWork1.exe" (
    set "EXE_PATH=build\Debug\LabWork1.exe"
)

if defined EXE_PATH (
    "%EXE_PATH%"
) else (
    echo [ERROR] LabWork1.exe not found!
    pause
    exit /b 1
)

echo.
echo Generating plots...
if exist plot_generator.py (
    python plot_generator.py
) else if exist doc\plot_generator.py (
    python doc\plot_generator.py
)

echo.
echo ==========================================
echo Program successfully completed.
echo ==========================================
cmd /k