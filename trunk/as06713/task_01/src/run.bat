
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
echo Project directory:
echo "%PROJECT_DIR%"
echo.

echo [1/4] Configuring CMake...
cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%"
if errorlevel 1 goto cmake_error

echo.
echo [2/4] Building project...
cmake --build "%BUILD_DIR%" --config Debug
if errorlevel 1 goto build_error

echo.
echo [3/4] Starting simulation...

if not exist "%EXE%" (
    echo ERROR: task_01.exe was not found:
    echo "%EXE%"
    goto finish
)

"%EXE%"

echo.
echo [4/4] Plotting results...

if exist "%PROJECT_DIR%\plot_results.py" (
    py -3 "%PROJECT_DIR%\plot_results.py"
) else (
    echo WARNING: plot_results.py was not found.
)

goto finish

:cmake_error
echo ERROR: CMake configuration failed.
goto finish

:build_error
echo ERROR: Build failed.
goto finish

:finish
echo.
echo Finished.
pause
