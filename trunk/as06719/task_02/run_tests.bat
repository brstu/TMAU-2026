@echo off
setlocal
cd /d "%~dp0"

echo ========================================
echo LAB 2 - AUTOMATED TESTING
echo ========================================

echo.
echo [1/4] Checking coverage tools...

where g++ >nul 2>nul
if errorlevel 1 goto tools_error

where gcov >nul 2>nul
if errorlevel 1 goto tools_error

where gcovr >nul 2>nul
if errorlevel 1 goto tools_error

echo Coverage tools found.

echo.
echo [2/4] Configuring GCC coverage build...

cmake -S . -B build_gcc -G "MinGW Makefiles" ^
  -DCMAKE_CXX_COMPILER="C:/msys64/ucrt64/bin/g++.exe" ^
  -DCMAKE_CXX_FLAGS="--coverage -O0 -g" ^
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"
if errorlevel 1 goto error

echo.
echo Removing old coverage data...

del /s /q "build_gcc\*.gcda" >nul 2>&1

echo.
echo Building tests...

cmake --build build_gcc
if errorlevel 1 goto error

echo.
echo [3/4] Running tests...

ctest --test-dir build_gcc --output-on-failure --output-junit build_gcc/test_report.xml
if errorlevel 1 goto error

echo.
echo [4/4] Generating coverage reports...

gcovr --root . --filter "src/" --exclude "src/main.cpp" --html-details build_gcc/coverage.html --xml-pretty --xml build_gcc/coverage.xml --print-summary build_gcc
if errorlevel 1 goto error

echo.
echo ========================================
echo ALL CHECKS COMPLETED SUCCESSFULLY
echo ========================================
echo.
echo Test report:    build_gcc\test_report.xml
echo HTML coverage:  build_gcc\coverage.html
echo XML coverage:   build_gcc\coverage.xml
echo.
pause
exit /b 0

:tools_error
echo.
echo ERROR: g++, gcov or gcovr was not found.
pause
exit /b 1

:error
echo.
echo ERROR: Build, tests or coverage generation failed.
pause
exit /b 1
