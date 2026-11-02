@echo off
setlocal EnableDelayedExpansion

rem Go to the folder where this .bat file is located (task_01)
cd /d "%~dp0"

rem ------------------------------------------------------------------
rem OPTIONAL: set these if the tools are installed in unusual folders
rem Example:  set "TOOLCHAIN=D:\w64devkit\bin"
rem Example:  set "CMAKE_DIR=D:\CMake\bin"
rem ------------------------------------------------------------------
set "TOOLCHAIN="
set "CMAKE_DIR="

echo ===================================================
echo  Variant 7 - build script
echo  Folder: %CD%
echo ===================================================
echo.

if defined TOOLCHAIN set "PATH=%TOOLCHAIN%;%PATH%"
if defined CMAKE_DIR set "PATH=%CMAKE_DIR%;%PATH%"

rem ================== 1. Find C++ compiler (g++) ==================
set "GXX_FOUND="
where g++ >nul 2>&1
if not errorlevel 1 set "GXX_FOUND=1"

if not defined GXX_FOUND (
    for %%D in ("C:\msys64\ucrt64\bin" "C:\msys64\mingw64\bin" "C:\w64devkit\bin" "D:\w64devkit\bin" "D:\qw\w64devkit\bin" "C:\mingw64\bin" "C:\MinGW\bin" "C:\winlibs\mingw64\bin" "%USERPROFILE%\w64devkit\bin" "C:\Program Files\CodeBlocks\MinGW\bin" "C:\Program Files (x86)\CodeBlocks\MinGW\bin") do (
        if not defined GXX_FOUND if exist "%%~D\g++.exe" (
            set "GXX_FOUND=1"
            set "PATH=%%~D;!PATH!"
            echo [INFO] Found g++ in %%~D
        )
    )
)
if not defined GXX_FOUND (
    for /d %%C in ("C:\Program Files\JetBrains\CLion*") do (
        if not defined GXX_FOUND if exist "%%~C\bin\mingw\bin\g++.exe" (
            set "GXX_FOUND=1"
            set "PATH=%%~C\bin\mingw\bin;!PATH!"
            echo [INFO] Found g++ in %%~C\bin\mingw\bin
        )
    )
)

rem ================== 2. Find CMake ==================
set "CMAKE_EXE="
where cmake >nul 2>&1
if not errorlevel 1 set "CMAKE_EXE=cmake"

if not defined CMAKE_EXE (
    for %%D in ("C:\Program Files\CMake\bin" "C:\Program Files (x86)\CMake\bin" "C:\msys64\ucrt64\bin" "C:\msys64\mingw64\bin" "C:\winlibs\mingw64\bin" "C:\mingw64\bin") do (
        if not defined CMAKE_EXE if exist "%%~D\cmake.exe" set "CMAKE_EXE=%%~D\cmake.exe"
    )
)
if not defined CMAKE_EXE (
    for /d %%C in ("C:\Program Files\JetBrains\CLion*") do (
        if not defined CMAKE_EXE if exist "%%~C\bin\cmake\win\x64\bin\cmake.exe" set "CMAKE_EXE=%%~C\bin\cmake\win\x64\bin\cmake.exe"
        if not defined CMAKE_EXE if exist "%%~C\bin\cmake\win\bin\cmake.exe" set "CMAKE_EXE=%%~C\bin\cmake\win\bin\cmake.exe"
    )
)

rem CMake bundled with Visual Studio
set "VS_PATH="
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
    for /f "usebackq delims=" %%I in (`"%VSWHERE%" -latest -products * -property installationPath`) do set "VS_PATH=%%I"
)
if not defined CMAKE_EXE if defined VS_PATH (
    if exist "!VS_PATH!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" set "CMAKE_EXE=!VS_PATH!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
)

if not defined CMAKE_EXE (
    echo [ERROR] CMake not found.
    echo         Install it, for example in cmd:  winget install Kitware.CMake
    echo         or download from https://cmake.org/download/
    echo         then close this window and run run_build.bat again.
    goto end
)
echo [INFO] CMake: !CMAKE_EXE!

if not defined GXX_FOUND if not defined VS_PATH (
    echo [ERROR] C++ compiler not found ^(neither g++ nor Visual Studio^).
    echo         Install MinGW, for example w64devkit or WinLibs,
    echo         or Visual Studio with "Desktop development with C++".
    echo         If g++ is already installed, put its bin folder into TOOLCHAIN at the top of this file.
    goto end
)

rem ================== 3. Choose generator ==================
set "GEN_ARGS="
if defined GXX_FOUND (
    where ninja >nul 2>&1
    if not errorlevel 1 (
        set "GEN_ARGS=-G Ninja -DCMAKE_CXX_COMPILER=g++"
        echo [INFO] Generator: Ninja + g++
    ) else (
        where mingw32-make >nul 2>&1
        if not errorlevel 1 (
            set "GEN_ARGS=-G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++"
        ) else (
            set "GEN_ARGS=-G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=make -DCMAKE_CXX_COMPILER=g++"
        )
        echo [INFO] Generator: MinGW Makefiles + g++
    )
) else (
    echo [INFO] Generator: Visual Studio
)
echo.

rem ================== 4. Clean old build ==================
taskkill /F /IM ControlModels.exe /T >nul 2>&1
if exist build rmdir /s /q build
mkdir build
if not exist doc mkdir doc

echo ===================================================
echo [STEP 1/4] Configuring project via CMake...
echo ===================================================
"!CMAKE_EXE!" -S . -B build !GEN_ARGS! -DCMAKE_BUILD_TYPE=Release
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
echo [STEP 3/4] Running simulation (Variant 7)...
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
    echo           CSV results are in the doc folder.
)

:end
echo.
echo ===================================================
echo Process finished. Press any key to exit.
echo ===================================================
pause
endlocal
