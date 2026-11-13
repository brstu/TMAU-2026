@echo off
setlocal EnableExtensions EnableDelayedExpansion
chcp 65001 >nul
cd /d "%~dp0"

echo ==========================================================
echo  TMAU, task 02: build, Google Test, coverage
echo ==========================================================

REM --- Stop stale processes that keep files inside build\ locked ---
REM A hung "cmake -P ...download-googletest-populate.cmake" process is the
REM usual reason why rmdir below cannot delete build\.
powershell -NoProfile -ExecutionPolicy Bypass -Command "$p = Get-CimInstance Win32_Process | Where-Object { $_.CommandLine -like '*task_02\build*' -and $_.Name -notmatch '^(powershell|pwsh|cmd|conhost|WindowsTerminal)' }; foreach ($x in $p) { try { Stop-Process -Id $x.ProcessId -Force -ErrorAction Stop } catch {} }" >nul 2>&1

REM --- Clean the build directory (with retries) ---
if exist build (
    for /l %%i in (1,1,3) do (
        if exist build rmdir /s /q build >nul 2>&1
        if exist build timeout /t 2 /nobreak >nul
    )
)

if exist build (
    echo [!] build is still locked, moving it aside to the temp folder...
    move build "%TEMP%\task02_stale_build_%RANDOM%" >nul 2>&1
)

if exist build (
    echo.
    echo [ERROR] Cannot remove or rename the build directory.
    echo         Close Visual Studio / Explorer windows on build\ and retry.
    goto :fail
)

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
    if exist "%~dp0doc\coverage\coverage_report.txt" (
        echo [i] Pre-generated GCC coverage report is available:
        echo     doc\coverage\coverage_report.txt
        echo     doc\coverage\coverage.html
        type "%~dp0doc\coverage\coverage_report.txt"
    )
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

cd /d "%~dp0"
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
%GCOVR_CMD% -r .. --filter "src/.*" --exclude ".*_deps.*" --exclude ".*googletest.*" --exclude "test/.*" --exclude ".*main\.cpp" --print-summary --txt-metric line --txt coverage_report.txt --html-details coverage.html --xml coverage.xml
if exist coverage_report.txt type coverage_report.txt
echo XML report: build\coverage.xml
echo HTML report: build\coverage.html
exit /b 0

:fail
cd /d "%~dp0"
echo.
echo ==========================================================
echo  FAILED: see the messages above.
echo ==========================================================
exit /b 1
