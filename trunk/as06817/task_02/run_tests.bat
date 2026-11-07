@echo off
setlocal EnableExtensions EnableDelayedExpansion
chcp 65001 >nul
cd /d "%~dp0"

echo ==========================================================
echo  TMAU, task 02: build, Google Test, coverage
echo ==========================================================

if exist build rmdir /s /q build
mkdir build
if errorlevel 1 goto :fail
cd build

REM --- Optional MinGW detection (coverage requires GNU tools) ---
if exist "C:\msys64\mingw64\bin\g++.exe" set "PATH=C:\msys64\mingw64\bin;%PATH%"
if exist "C:\mingw64\bin\g++.exe" set "PATH=C:\mingw64\bin;%PATH%"
if exist "C:\Program Files\mingw64\bin\g++.exe" set "PATH=C:\Program Files\mingw64\bin;%PATH%"

set "GENERATOR_ARGS="
set "COVERAGE=OFF"

where g++ >nul 2>nul
if not errorlevel 1 (
    where mingw32-make >nul 2>nul
    if not errorlevel 1 (
        set "GENERATOR_ARGS=-G "MinGW Makefiles""
        set "COVERAGE=ON"
    )
)

if "%GENERATOR_ARGS%"=="" (
    echo [i] MinGW not found: using the default CMake generator ^(Visual Studio^).
    echo [i] Coverage report requires MinGW + gcovr and will be skipped.
)

cmake .. %GENERATOR_ARGS% -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=%COVERAGE%
if errorlevel 1 goto :fail

cmake --build . --config Debug --parallel
if errorlevel 1 goto :fail

echo.
echo ==========================================================
echo  Running Google Test
echo ==========================================================
ctest -C Debug --output-on-failure --output-junit test_report.xml > test_report.txt 2>&1
set "TEST_EXIT_CODE=%ERRORLEVEL%"
type test_report.txt
if not "%TEST_EXIT_CODE%"=="0" goto :fail

if /I "%COVERAGE%"=="ON" call :coverage

cd ..
echo.
echo ==========================================================
echo  SUCCESS: all tests passed.
echo  Reports: build\test_report.txt, build\test_report.xml
echo ==========================================================
exit /b 0

:coverage
echo.
echo ==========================================================
echo  Generating coverage report with gcovr
echo ==========================================================
set "GCOVR_CMD=gcovr"
where gcovr >nul 2>nul
if not errorlevel 1 goto :coverage_run

echo [i] gcovr not found, trying to install it for the current user...
python -m pip install --user gcovr >nul 2>&1
where gcovr >nul 2>nul
if not errorlevel 1 goto :coverage_run

python -m gcovr --version >nul 2>nul
if errorlevel 1 (
    echo [WARNING] gcovr is unavailable, coverage report skipped.
    exit /b 0
)
set "GCOVR_CMD=python -m gcovr"

:coverage_run
%GCOVR_CMD% -r .. --filter "src/.*" --exclude ".*_deps.*" --exclude ".*googletest.*" --exclude "test/.*" --exclude ".*main\.cpp" --txt-metric line --txt coverage_report.txt --html-details coverage.html
if exist coverage_report.txt type coverage_report.txt
echo HTML report: build\coverage.html
exit /b 0

:fail
cd ..
echo.
echo ==========================================================
echo  FAILED: see the messages above.
echo ==========================================================
exit /b 1
