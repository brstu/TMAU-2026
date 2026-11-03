
@echo off
setlocal

for %%I in ("%~dp0.") do set "PROJECT_DIR=%%~fI"
set "BUILD_DIR=%PROJECT_DIR%\out\build\x64-Debug"
set "EXE=%BUILD_DIR%\Debug\task_01.exe"

cd /d "%PROJECT_DIR%"

echo ==========================================
echo Lab 1 - Variant 20
echo CMake build and interactive simulation
echo ==========================================
echo.

echo [1/3] Configuring CMake...
cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%"
if errorlevel 1 goto cmake_error

echo.
echo [2/3] Building project...
cmake --build "%BUILD_DIR%" --config Debug
if errorlevel 1 goto build_error

echo.
echo [3/3] Starting simulation...

if not exist "%EXE%" (
    echo ERROR: task_01.exe was not found.
    goto finish
)

"%EXE%"

goto finish

:cmake_error
echo ERROR: CMake configuration failed.
goto finish

:build_error
echo ERROR: Build failed.
goto finish

:finish
echo.
echo ==========================================
echo Finished.
echo ==========================================
pause
