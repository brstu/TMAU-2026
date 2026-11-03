@echo off
setlocal
chcp 65001 >nul

REM Build and run Task 01 (Variant 17)
cd /d "%~dp0"

where cmake >nul 2>nul
if errorlevel 1 goto no_cmake

set "BUILD_DIR=%~dp0build"
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if errorlevel 1 goto failed

echo [1/3] Configuring project...
REM Run CMake from the build directory for compatibility with CMake launchers.
pushd "%BUILD_DIR%"
cmake "%~dp0."
if errorlevel 1 (
    popd
    goto failed
)

echo [2/3] Building project...
cmake --build . --config Release
if errorlevel 1 (
    popd
    goto failed
)
popd

echo [3/3] Running program...
if exist "%BUILD_DIR%\Release\task_01.exe" (
    "%BUILD_DIR%\Release\task_01.exe"
) else if exist "%BUILD_DIR%\task_01.exe" (
    "%BUILD_DIR%\task_01.exe"
) else (
    echo ERROR: task_01.exe was not created.
    goto failed
)

if errorlevel 1 goto failed

if exist "%~dp0plot_results.py" (
    where python >nul 2>nul
    if not errorlevel 1 (
        echo.
        echo Opening plot...
        python "%~dp0plot_results.py"
        if errorlevel 1 echo Plot was not opened. To enable it, run: python -m pip install matplotlib
    ) else (
        echo Python was not found. The CSV results were saved; plot is skipped.
    )
)

echo.
echo Finished.
pause
exit /b 0

:no_cmake
echo ERROR: CMake was not found. Install CMake and add it to PATH.
echo Then close and reopen Command Prompt and run build.bat again.
pause
exit /b 1

:failed
echo.
echo BUILD OR RUN FAILED. Read the error messages above.
pause
exit /b 1
