@echo off
setlocal
chcp 65001 >nul

for %%I in ("%~dp0.") do set "SOURCE_DIR=%%~fI"
set "BUILD_DIR=%SOURCE_DIR%\build"

echo ==============================================
echo BUILDING VARIANT 8 PROJECT
echo ==============================================

where cmake >nul 2>nul
if errorlevel 1 (
    echo ERROR: CMake was not found in PATH.
    echo Install CMake and add it to PATH.
    pause
    exit /b 1
)

echo.
echo [1/2] CONFIGURING PROJECT...
echo Source: "%SOURCE_DIR%"
echo Build:  "%BUILD_DIR%"
cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%"
if errorlevel 1 (
    echo ERROR: CMake configuration failed.
    pause
    exit /b 1
)

echo.
echo [2/2] BUILDING PROGRAM...
cmake --build "%BUILD_DIR%" --config Release --parallel
if errorlevel 1 (
    echo ERROR: Program build failed.
    pause
    exit /b 1
)

set "PROGRAM=%BUILD_DIR%\Release\model_simulator.exe"
if not exist "%PROGRAM%" set "PROGRAM=%BUILD_DIR%\model_simulator.exe"
if not exist "%PROGRAM%" set "PROGRAM=%BUILD_DIR%\Debug\model_simulator.exe"
if not exist "%PROGRAM%" (
    echo ERROR: Executable file was not found.
    pause
    exit /b 1
)

echo.
echo ==============================================
echo STARTING SIMULATION
echo ==============================================
"%PROGRAM%"
set "PROGRAM_EXIT_CODE=%ERRORLEVEL%"

echo.
echo Simulation process finished with code %PROGRAM_EXIT_CODE%.
pause
exit /b %PROGRAM_EXIT_CODE%
