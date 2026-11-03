@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0"




taskkill /F /IM MSBuild.exe     >nul 2>nul
taskkill /F /IM VBCSCompiler.exe >nul 2>nul


set "CMAKE_EXE="
where cmake >nul 2>nul && set "CMAKE_EXE=cmake"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_PATH="
set "VS_LINE="
if exist "!VSWHERE!" (
    for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_PATH=%%i"
    for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property catalog_productLineVersion`) do set "VS_LINE=%%i"
)
if not defined CMAKE_EXE if defined VS_PATH (
    if exist "!VS_PATH!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" (
        set "CMAKE_EXE=!VS_PATH!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
    )
)
if not defined CMAKE_EXE (
    echo ERROR: cmake.exe not found.
    echo   Install CMake or add it to PATH.
    pause
    exit /b 1
)
if not defined VS_PATH (
    echo ERROR: Visual Studio with C++ tools not found.
    echo   Install "Desktop development with C++" workload.
    pause
    exit /b 1
)

set "GENERATOR="
if "!VS_LINE!"=="2022" set "GENERATOR=Visual Studio 17 2022"
if "!VS_LINE!"=="2019" set "GENERATOR=Visual Studio 16 2019"
if "!VS_LINE!"=="2017" set "GENERATOR=Visual Studio 15 2017"
if not defined GENERATOR (
    echo ERROR: unsupported VS product line "!VS_LINE!".
    pause
    exit /b 1
)


if exist build (
    rmdir /s /q build 2>nul
    if exist build (
        echo WARNING: cannot clean build\ - using existing folder.
        echo          Close Visual Studio if it holds build\task_01.sln
    )
)
if not exist build mkdir build
cd build

echo [1/3] Running CMake configure...
echo        Generator: !GENERATOR!
"!CMAKE_EXE!" .. -G "!GENERATOR!" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo CMake configure failed!
    cd ..
    pause
    exit /b %ERRORLEVEL%
)

echo.

echo [2/3] Building...
"!CMAKE_EXE!" --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    cd ..
    pause
    exit /b %ERRORLEVEL%
)

echo.

echo [2.5/3] Copying render_plot.py next to exe...
if exist ..\render_plot.py copy /Y ..\render_plot.py Release\render_plot.py >nul
if exist Release\render_plot.py echo        OK: build\Release\render_plot.py

echo.

echo [3/3] Running demo...
chcp 65001 >nul
if exist Release\task_01.exe (
    Release\task_01.exe
) else (
    task_01.exe
)
if %ERRORLEVEL% NEQ 0 echo Program exited with code %ERRORLEVEL%

echo.
echo Build and run completed successfully.
cd ..
pause
endlocal