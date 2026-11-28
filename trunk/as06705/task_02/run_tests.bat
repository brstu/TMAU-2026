@echo off
setlocal enabledelayedexpansion

cd /d "%~dp0"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "!VSWHERE!" (
    echo ERROR: vswhere.exe not found. Install Visual Studio.
    pause & exit /b 1
)

set "VS_PATH="
set "VS_LINE="
for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_PATH=%%i"
for /f "usebackq tokens=*" %%i in (`"!VSWHERE!" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property catalog_productLineVersion`) do set "VS_LINE=%%i"

if not defined VS_PATH (
    echo ERROR: Visual Studio with C++ tools not found.
    pause & exit /b 1
)
echo [OK] VS path: !VS_PATH!
echo [OK] VS line: !VS_LINE!

set "CMAKE_EXE=!VS_PATH!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
if not exist "!CMAKE_EXE!" (
    where cmake >nul 2>nul && set "CMAKE_EXE=cmake"
)
if not exist "!CMAKE_EXE!" (
    echo ERROR: cmake.exe not found.
    pause & exit /b 1
)
echo [OK] CMake: !CMAKE_EXE!

set "GENERATOR="
if "!VS_LINE!"=="2022" set "GENERATOR=Visual Studio 17 2022"
if "!VS_LINE!"=="2019" set "GENERATOR=Visual Studio 16 2019"
if not defined GENERATOR (
    echo ERROR: unsupported VS product line "!VS_LINE!".
    pause & exit /b 1
)
echo [OK] Generator: !GENERATOR!
echo.

if exist build_tests rmdir /s /q build_tests 2>nul
mkdir build_tests
cd build_tests

echo [1/3] CMake configure...
"!CMAKE_EXE!" ../src -G "!GENERATOR!" -A x64
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: configure failed.
    cd .. & pause & exit /b 1
)

echo.
echo [2/3] Building...
"!CMAKE_EXE!" --build . --config RelWithDebInfo
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: build failed.
    cd .. & pause & exit /b 1
)

echo.
echo [3/3] Running tests...

for %%F in ("!CMAKE_EXE!") do set "CMAKE_DIR=%%~dpF"
set "CTEST_EXE=!CMAKE_DIR!ctest.exe"
if not exist "!CTEST_EXE!" (
    where ctest >nul 2>nul && set "CTEST_EXE=ctest"
)

if defined CTEST_EXE (
    "!CTEST_EXE!" --output-on-failure -C RelWithDebInfo
    set "TEST_RESULT=!ERRORLEVEL!"
) else (
    echo ERROR: ctest.exe not found.
    set "TEST_RESULT=1"
)

echo.
where OpenCppCoverage >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [Coverage] Running OpenCppCoverage...
    if exist "RelWithDebInfo\task02_tests.exe" (
        
        set "ABS_LAB1="
        if exist "..\..\task_01\src" for %%I in ("..\..\task_01\src") do set "ABS_LAB1=%%~fI"
        if exist "..\..\task_01\scr" for %%I in ("..\..\task_01\scr") do set "ABS_LAB1=%%~fI"
        
        for %%I in ("..\src") do set "ABS_LAB2=%%~fI"
        for %%I in ("RelWithDebInfo\task02_tests.exe") do set "ABS_EXE=%%~fI"

        OpenCppCoverage ^
            --sources "!ABS_LAB1!" ^
            --sources "!ABS_LAB2!" ^
            --modules "!ABS_EXE!" ^
            --export_type html:coverage_html ^
            --export_type cobertura:coverage.xml ^
            -- RelWithDebInfo\task02_tests.exe
        
        if !ERRORLEVEL! EQU 0 (
            echo [OK] Coverage report: build_tests\coverage_html\index.html
            rem === АВТОМАТИЧЕСКИ ОТКРЫВАЕМ ОТЧЕТ В БРАУЗЕРЕ ===
            start "" "coverage_html\index.html"
        ) else (
            echo [WARN] OpenCppCoverage failed.
        )
    )
) else (
    echo [SKIP] OpenCppCoverage not installed.
)

cd ..

echo.
echo =============================================
if !TEST_RESULT! EQU 0 (
    echo  SUCCESS: all tests passed.
) else (
    echo  FAILURE: some tests failed.
)
echo =============================================
pause
endlocal
exit /b !TEST_RESULT!