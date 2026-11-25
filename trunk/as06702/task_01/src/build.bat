@echo off
chcp 65001 >nul
setlocal
set "PROJECT_DIR=%~dp0"
if "%PROJECT_DIR:~-1%"=="\" set "PROJECT_DIR=%PROJECT_DIR:~0,-1%"
set "BUILD_DIR=%PROJECT_DIR%\out\build-bat"

cd /d "%PROJECT_DIR%"

echo ================================================
echo Building Variant 13 Project with CMake
echo ================================================

where cmake >nul 2>&1
if errorlevel 1 (
    echo ERROR: CMake not found. Please install CMake and add it to PATH.
    pause
    exit /b 1
)

echo.
echo [1/3] Configuring CMake...
cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%"
if errorlevel 1 (
    echo ERROR during CMake configuration.
    pause
    exit /b 1
)

echo.
echo [2/3] Building C++ program...
cmake --build "%BUILD_DIR%" --config Debug --verbose
if errorlevel 1 (
    echo ERROR during program build.
    pause
    exit /b 1
)

echo.
echo [3/3] Running simulation and plotting...
if exist "%BUILD_DIR%\Debug\controlled_object.exe" (
    "%BUILD_DIR%\Debug\controlled_object.exe"
) else if exist "%BUILD_DIR%\controlled_object.exe" (
    "%BUILD_DIR%\controlled_object.exe"
) else (
    echo Error: controlled_object.exe not found in build directory.
    pause
    exit /b 1
)

echo.
echo Done.
pause