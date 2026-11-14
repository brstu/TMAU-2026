@echo off
title task_02 - build and run unit tests
setlocal
cd /d "%~dp0"

where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo.
    echo  *** CMake not found in PATH! ***
    echo  Install: winget install -e --id Kitware.CMake
    echo.
    pause
    exit /b 1
)

set "GENERATOR="
set "COV_FLAGS="
where g++ >nul 2>nul
if %errorlevel% neq 0 goto use_default_gen
where mingw32-make >nul 2>nul
if %errorlevel% neq 0 goto use_default_gen
set GENERATOR=-G "MinGW Makefiles"
set COV_FLAGS=-DCMAKE_CXX_FLAGS="-g -O0 --coverage"
echo Toolchain: MinGW GCC - tests will be built with gcov coverage flags
goto toolchain_ok
:use_default_gen
echo Toolchain: default generator (e.g. Visual Studio).
echo g++ or mingw32-make not found in PATH - gcov coverage will be SKIPPED.
:toolchain_ok

echo =======================================
echo  [1/3] Configure + build (Google Test)
echo =======================================
cmake -S . -B build %GENERATOR% %COV_FLAGS%
if errorlevel 1 goto error

cmake --build build --config Release
if errorlevel 1 goto error

echo.
echo =======================================
echo  [2/3] Run tests with ctest
echo =======================================
cd build
ctest --output-on-failure > ..\test_report.txt 2>&1
set RESULT=%errorlevel%
type ..\test_report.txt
cd ..

echo.
echo =======================================
echo  [3/3] Coverage report (gcovr)
echo =======================================
if not defined COV_FLAGS goto cov_no_gcc

REM gcovr запускается из папки выше (..): там пути выглядят как task_01/src/... и task_02/src/...
REM main.cpp (ввод-вывод с клавиатуры) в замер не входит.
pushd ..
python -m gcovr -r . --filter ".*task_01/src/" --filter ".*task_02/src/" --exclude ".*main[.]cpp" --print-summary task_02/build > task_02\coverage_report.txt
popd
type coverage_report.txt
pushd ..
python -m gcovr -r . --filter ".*task_01/src/" --filter ".*task_02/src/" --exclude ".*main[.]cpp" --html-details task_02/coverage.html task_02/build || echo  (gcovr: HTML-report not created)
popd
goto cov_done

:cov_no_gcc
echo  Skipped: build not GCC, no gcov files.
:cov_done

if %RESULT% neq 0 goto error
echo.
echo  All tests passed! Reports: test_report.txt, coverage_report.txt, coverage.html
echo.
pause
exit /b 0

:error
echo.
echo *** FAILED! See output above. ***
echo.
pause
exit /b 1
