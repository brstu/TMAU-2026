@echo off
title task_02 - build and run unit tests
setlocal
cd /d "%~dp0"

where cmake >nul 2>nul
if errorlevel 1 (
    echo.
    echo  *** CMake not found in PATH! ***
    echo  Install: winget install -e --id Kitware.CMake
    echo.
    pause
    exit /b 1
)

REM выбираем python-лаунчер: py, если есть, иначе python
set "PY=python"
where py >nul 2>nul && set "PY=py"

set "GEN="
set "COV="
where g++ >nul 2>nul
if errorlevel 1 goto def_gen
where mingw32-make >nul 2>nul
if errorlevel 1 goto def_gen
set GEN=-G "MinGW Makefiles"
set COV=-DCMAKE_CXX_FLAGS="-g -O0 --coverage"
echo Toolchain: MinGW GCC - tests will be built with gcov coverage flags
goto gen_ok

:def_gen
echo Toolchain: default generator (e.g. Visual Studio).
echo g++ or mingw32-make not found in PATH - gcov coverage will be SKIPPED.

:gen_ok
echo =======================================
echo  [1/3] Configure + build (Google Test)
echo =======================================
cmake -S . -B build %GEN% %COV%
if errorlevel 1 goto fail
cmake --build build --config Release
if errorlevel 1 goto fail

echo.
echo =======================================
echo  [2/3] Run tests with ctest
echo =======================================
cd build
ctest --output-on-failure > ..\test_report.txt 2>&1
set RC=%errorlevel%
type ..\test_report.txt
cd ..

echo.
echo =======================================
echo  [3/3] Coverage report (gcovr)
echo =======================================
if not defined COV goto no_cov
pushd ..
%PY% -m gcovr -r . --filter ".*task_01/src/" --filter ".*task_02/src/" --exclude ".*main[.]cpp" --print-summary task_02/build > task_02\coverage_report.txt
popd
type coverage_report.txt
pushd ..
%PY% -m gcovr -r . --filter ".*task_01/src/" --filter ".*task_02/src/" --exclude ".*main[.]cpp" --html-details task_02/coverage.html task_02/build || echo  (gcovr: HTML-report not created)
popd
goto cov_ok

:no_cov
echo  Skipped: build not GCC, no gcov files.

:cov_ok
if %RC% neq 0 goto fail
echo.
echo  All tests passed! Reports: test_report.txt, coverage_report.txt, coverage.html
echo.
pause
exit /b 0

:fail
echo.
echo *** FAILED! See output above. ***
echo.
pause
exit /b 1