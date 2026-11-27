@echo off
setlocal

:: Корень проекта
cd /d "%~dp0"

set "CMAKE_EXE=C:\Program Files\CMake\bin\cmake.exe"
set "CTEST_EXE=C:\Program Files\CMake\bin\ctest.exe"
set "TOOLCHAIN=D:\qw\w64devkit\bin"
set "PATH=%TOOLCHAIN%;%PATH%"

if not exist doc mkdir doc

echo ===================================================
echo [1/4] Clean build directory...
echo ===================================================
if exist build rmdir /s /q build
mkdir build

echo.
echo ===================================================
echo [2/4] Configuring CMake (Ninja, coverage=ON)...
echo ===================================================
"%CMAKE_EXE%" -S . -B build -G Ninja ^
    -DCMAKE_C_COMPILER:FILEPATH="%TOOLCHAIN%\gcc.exe" ^
    -DCMAKE_CXX_COMPILER:FILEPATH="%TOOLCHAIN%\g++.exe" ^
    -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 goto :fail

echo.
echo ===================================================
echo [3/4] Building project and tests...
echo ===================================================
"%CMAKE_EXE%" --build build -j 4
if errorlevel 1 goto :fail

echo.
echo ===================================================
echo [4/4] Running Google Test via CTest...
echo ===================================================
"%CTEST_EXE%" --test-dir build --output-on-failure > doc\test_report.txt 2>&1
set "TEST_RC=%errorlevel%"
type doc\test_report.txt
if not "%TEST_RC%"=="0" goto :fail

echo.
echo ===================================================
echo Coverage report (gcov)...
echo ===================================================
py coverage.py --gcov "%TOOLCHAIN%\gcov.exe"
if errorlevel 1 goto :fail

echo.
echo Report: doc\test_report.txt, doc\coverage_report.txt
echo ALL TESTS PASSED.
if /i not "%~1"=="--ci" pause
endlocal
exit /b 0

:fail
echo.
echo [ERROR] Build, tests or coverage step failed. See doc\test_report.txt
if /i not "%~1"=="--ci" pause
endlocal
exit /b 1
