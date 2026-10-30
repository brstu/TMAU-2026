@echo off
setlocal EnableDelayedExpansion

rem Go to the folder where this .bat file is located (task_01)
cd /d "%~dp0"

rem ------------------------------------------------------------------
rem OPTIONAL: path to your compiler (MinGW / w64devkit) if it is not in PATH
rem Example:  set "TOOLCHAIN=D:\w64devkit\bin"
rem ------------------------------------------------------------------
set "TOOLCHAIN="
if defined TOOLCHAIN set "PATH=%TOOLCHAIN%;%PATH%"

echo ===================================================
echo  Variant 15 - build script
echo  Folder: %CD%
echo ===================================================
echo.

rem ---------- Find CMake ----------
set "CMAKE_EXE="
where cmake >nul 2>&1
if not errorlevel 1 set "CMAKE_EXE=cmake"
if not defined CMAKE_EXE if exist "C:\Program Files\CMake\bin\cmake.exe" set "CMAKE_EXE=C:\Program Files\CMake\bin\cmake.exe"
if not defined CMAKE_EXE (
    echo [ERROR] CMake not found. Install CMake or add it to PATH.
    goto end
)
echo [INFO] CMake: !CMAKE_EXE!

rem ---------- Choose generator: Ninja, then MinGW Makefiles, then default ----------
set "GENERATOR="
where ninja >nul 2>&1
if not errorlevel 1 set "GENERATOR=Ninja"
if not defined GENERATOR (
    where mingw32-make >nul 2>&1
    if not errorlevel 1 set "GENERATOR=MinGW Makefiles"
)
if defined GENERATOR (
    echo [INFO] Generator: !GENERATOR!
) else (
    echo [INFO] Generator: CMake default ^(Visual Studio^)
)
echo.

rem ---------- Clean old build ----------
taskkill /F /IM ControlModels.exe /T >nul 2>&1
if exist build rmdir /s /q build
mkdir build
if not exist doc mkdir doc

echo ===================================================
echo [STEP 1/4] Configuring project via CMake...
echo ===================================================
if defined GENERATOR (
    "!CMAKE_EXE!" -S . -B build -G "!GENERATOR!" -DCMAKE_BUILD_TYPE=Release
) else (
    "!CMAKE_EXE!" -S . -B build -DCMAKE_BUILD_TYPE=Release
)
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    goto end
)

echo.
echo ===================================================
echo [STEP 2/4] Building project...
echo ===================================================
"!CMAKE_EXE!" --build build --config Release --verbose
if errorlevel 1 (
    echo [ERROR] Compilation failed.
    goto end
)

rem Exe path differs for Ninja/MinGW and Visual Studio
set "EXE=build\ControlModels.exe"
if not exist "!EXE!" set "EXE=build\Release\ControlModels.exe"
if not exist "!EXE!" (
    echo [ERROR] ControlModels.exe not found after build.
    goto end
)

echo.
echo ===================================================
echo [STEP 3/4] Running simulation (Variant 15)...
echo ===================================================
rem Run from task_01 so that CSV files go to doc\
"!EXE!"
if errorlevel 1 (
    echo [ERROR] Simulation failed.
    goto end
)

echo.
echo ===================================================
echo [STEP 4/4] Generating graphs (Python/Matplotlib)...
echo ===================================================
set "PYTHON_EXE="
where py >nul 2>&1
if not errorlevel 1 set "PYTHON_EXE=py"
if not defined PYTHON_EXE (
    where python >nul 2>&1
    if not errorlevel 1 set "PYTHON_EXE=python"
)
if defined PYTHON_EXE (
    !PYTHON_EXE! src\plot.py
) else (
    echo [WARNING] Python not found - graphs were not generated.
)

:end
echo.
echo ===================================================
echo Process finished. Press any key to exit.
echo ===================================================
pause
endlocal
