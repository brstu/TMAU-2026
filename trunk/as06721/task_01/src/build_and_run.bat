@echo off
setlocal EnableExtensions EnableDelayedExpansion

chcp 65001 >nul
set "PYTHONUTF8=1"
set "PYTHONIOENCODING=utf-8"

cd /d "%~dp0"

echo ========================================
echo   [task_01] Build and run
echo ========================================
echo.

if exist "build" rmdir /s /q "build"
mkdir "build"

rem ------------------------------------------------------------
rem 1. Try to initialize a modern Visual Studio environment.
rem ------------------------------------------------------------
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        set "VSINSTALL=%%I"
    )

    if defined VSINSTALL (
        if exist "!VSINSTALL!\Common7\Tools\VsDevCmd.bat" (
            call "!VSINSTALL!\Common7\Tools\VsDevCmd.bat" -arch=x64 >nul 2>&1
        )
    )
)

rem ------------------------------------------------------------
rem 2. Visual Studio 2015 / MSBuild 14 fallback.
rem ------------------------------------------------------------
if exist "%ProgramFiles(x86)%\MSBuild\14.0\Bin\MSBuild.exe" (
    set "PATH=%ProgramFiles(x86)%\MSBuild\14.0\Bin;%PATH%"
)

if exist "%ProgramFiles%\MSBuild\14.0\Bin\MSBuild.exe" (
    set "PATH=%ProgramFiles%\MSBuild\14.0\Bin;%PATH%"
)

if defined VS140COMNTOOLS (
    if exist "%VS140COMNTOOLS%VsDevCmd.bat" (
        call "%VS140COMNTOOLS%VsDevCmd.bat" >nul 2>&1
    )
)

rem ------------------------------------------------------------
rem 3. Preferred build: CMake + NMake/MSVC.
rem ------------------------------------------------------------
where cmake >nul 2>&1
if not errorlevel 1 (
    where nmake >nul 2>&1
    if not errorlevel 1 (
        echo [1/3] Configuring with MSVC/NMake...
        cmake -S . -B build -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
        if not errorlevel 1 (
            echo.
            echo [2/3] Building...
            cmake --build build
            if not errorlevel 1 (
                set "APP=build\as06722_task01.exe"
                goto run_app
            )
        )

        echo.
        echo MSVC/NMake build failed. Trying another compiler...
        if exist "build" rmdir /s /q "build"
        mkdir "build"
    )
)

rem ------------------------------------------------------------
rem 4. Fallback: compile directly with g++.
rem ------------------------------------------------------------
where g++ >nul 2>&1
if not errorlevel 1 (
    echo [1/3] Compiling directly with g++...
    g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -municode ^
        -DPROJECT_SOURCE_DIR=\".\" ^
        main.cpp -o build\as06722_task01.exe

    if errorlevel 1 (
        rem Some MinGW installations do not accept -municode here.
        g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic ^
            -DPROJECT_SOURCE_DIR=\".\" ^
            main.cpp -o build\as06722_task01.exe
    )

    if not errorlevel 1 (
        echo.
        echo [2/3] Build completed.
        set "APP=build\as06722_task01.exe"
        goto run_app
    )
)

rem ------------------------------------------------------------
rem 5. Last CMake attempt: use the available default generator.
rem    MSBuild 14 was added to PATH above for Visual Studio 2015.
rem ------------------------------------------------------------
where cmake >nul 2>&1
if not errorlevel 1 (
    echo [1/3] Configuring with the available CMake generator...
    if exist "build" rmdir /s /q "build"
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

    if not errorlevel 1 (
        echo.
        echo [2/3] Building...
        cmake --build build --config Release

        if not errorlevel 1 (
            if exist "build\Release\as06722_task01.exe" (
                set "APP=build\Release\as06722_task01.exe"
                goto run_app
            )

            if exist "build\as06722_task01.exe" (
                set "APP=build\as06722_task01.exe"
                goto run_app
            )
        )
    )
)

echo.
echo [ERROR] The project could not be compiled.
echo.
echo A C++ compiler is required.
echo Supported options:
echo   - Visual Studio / Build Tools with C++ workload
echo   - MinGW-w64 g++
echo.
goto error

:run_app
if not exist "%APP%" (
    echo [ERROR] Executable was not created.
    goto error
)

echo.
echo [3/3] Running program...
echo.
"%APP%"
if errorlevel 1 goto error

echo.
echo ========================================
echo Program finished successfully.
echo ========================================
pause
exit /b 0

:error
echo.
echo ========================================
echo Build or program execution failed.
echo Read the messages above.
echo ========================================
pause
exit /b 1
