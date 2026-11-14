@echo off
setlocal EnableExtensions
chcp 65001 >nul
cd /d "%~dp0"

echo ================================================
echo   TiMAY LR2 - build and Google Test - Variant 13
echo ================================================

set "BUILD_DIR=build_tests"
set "COVERAGE_DIR=coverage\html"
set "INDEX_FILE=%CD%\coverage\html\index.html"

where cmake >nul 2>nul
if errorlevel 1 (
    echo ERROR: CMake was not found in PATH.
    goto failed
)

echo.
echo [1/4] Configuring CMake and downloading Google Test if required...
cmake -S . -B "%BUILD_DIR%" -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 goto failed

echo.
echo [2/4] Building application and unit tests...
cmake --build "%BUILD_DIR%" --config Debug --parallel
if errorlevel 1 goto failed

echo.
echo [3/4] Running Google Test...
ctest --test-dir "%BUILD_DIR%" -C Debug --output-on-failure --output-log test_results.txt
if errorlevel 1 goto failed

echo.
echo [4/4] Generating coverage report...
if not exist coverage mkdir coverage

set "OPENCOV="
where OpenCppCoverage >nul 2>nul
if not errorlevel 1 set "OPENCOV=OpenCppCoverage"

if not defined OPENCOV (
    if exist "%ProgramFiles%\OpenCppCoverage\OpenCppCoverage.exe" (
        set "OPENCOV=%ProgramFiles%\OpenCppCoverage\OpenCppCoverage.exe"
    )
)

if defined OPENCOV (
    if exist "%BUILD_DIR%\bin\unit_tests.exe" (
        "%OPENCOV%" --sources "%CD%\src" --modules "%CD%\%BUILD_DIR%\bin\unit_tests.exe" --export_type html:"%CD%\%COVERAGE_DIR%" -- "%CD%\%BUILD_DIR%\bin\unit_tests.exe"
        if errorlevel 1 echo WARNING: OpenCppCoverage could not complete.
    ) else (
        echo WARNING: unit_tests.exe was not found at %BUILD_DIR%\bin.
    )
) else (
    echo OpenCppCoverage was not found.
    echo Install it to generate the HTML coverage report.
)

echo.
echo ================================================
echo   Opening coverage report...
echo ================================================
echo.

if exist "%INDEX_FILE%" (
    start "" "%INDEX_FILE%"
    echo Coverage report opened in browser.
    echo Report: %INDEX_FILE%
) else (
    echo WARNING: coverage\html\index.html was not found.
    echo Check the OpenCppCoverage output above.
)

echo.
echo ================================================
echo   Build and tests completed.
echo ================================================
goto finish

:failed
echo.
echo ERROR: Build or tests failed. Read the messages above.
goto finish

:finish
echo.
pause
endlocal