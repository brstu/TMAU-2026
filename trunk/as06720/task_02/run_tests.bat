@echo off
title task_02 (as06720, variant 15) - build and run unit tests
setlocal EnableDelayedExpansion
cd /d "%~dp0"

rem ------------------------------------------------------------------
rem OPTIONAL: path to your compiler (MinGW / w64devkit) if it is not in PATH
rem Example:  set "TOOLCHAIN=D:\w64devkit\bin"
rem ------------------------------------------------------------------
set "TOOLCHAIN="
if defined TOOLCHAIN set "PATH=%TOOLCHAIN%;%PATH%"

echo ===================================================
echo  Lab 2, variant 15 - unit tests (Google Test/Mock)
echo  Folder: %CD%
echo ===================================================

rem ---------- Find CMake ----------
set "CMAKE_EXE="
where cmake >nul 2>&1
if not errorlevel 1 set "CMAKE_EXE=cmake"
if not defined CMAKE_EXE if exist "C:\Program Files\CMake\bin\cmake.exe" set "CMAKE_EXE=C:\Program Files\CMake\bin\cmake.exe"
if not defined CMAKE_EXE (
    echo [ERROR] CMake not found. Install: winget install -e --id Kitware.CMake
    goto error
)
for %%I in ("!CMAKE_EXE!") do set "CTEST_EXE=%%~dpIctest.exe"
if not exist "!CTEST_EXE!" set "CTEST_EXE=ctest"
echo [INFO] CMake: !CMAKE_EXE!

rem ---------- Toolchain: GCC (MinGW) gives coverage, otherwise default generator ----------
set "GENERATOR="
set "COVERAGE=0"
where g++ >nul 2>&1
if errorlevel 1 goto default_gen
where ninja >nul 2>&1
if not errorlevel 1 set "GENERATOR=Ninja"
if not defined GENERATOR (
    where mingw32-make >nul 2>&1
    if not errorlevel 1 set "GENERATOR=MinGW Makefiles"
)
if not defined GENERATOR goto default_gen
set "COVERAGE=1"
echo [INFO] Generator: !GENERATOR!, gcov coverage flags ON
goto toolchain_ok
:default_gen
echo [INFO] Generator: CMake default (Visual Studio). gcov coverage will be SKIPPED.
:toolchain_ok

if exist build rmdir /s /q build

echo.
echo ===================================================
echo  [1/3] Configure + build (Google Test is downloaded by CMake)
echo ===================================================
if "!COVERAGE!"=="1" (
    "!CMAKE_EXE!" -S . -B build -G "!GENERATOR!" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-g -O0 --coverage"
) else (
    "!CMAKE_EXE!" -S . -B build
)
if errorlevel 1 goto error

"!CMAKE_EXE!" --build build --config Debug
if errorlevel 1 goto error

echo.
echo ===================================================
echo  [2/3] Run tests (ctest)
echo ===================================================
pushd build
"!CTEST_EXE!" -C Debug --output-on-failure --output-junit ..\test_report.xml > ..\test_report.txt 2>&1
set "RESULT=!errorlevel!"
popd
type test_report.txt

echo.
echo ===================================================
echo  [3/3] Coverage report (gcovr)
echo ===================================================
if not "!COVERAGE!"=="1" (
    echo  Skipped: not a GCC build, no gcov data.
    goto after_cov
)

set "PY="
where py >nul 2>&1
if not errorlevel 1 set "PY=py"
if not defined PY (
    where python >nul 2>&1
    if not errorlevel 1 set "PY=python"
)
if not defined PY (
    echo  [WARNING] Python not found - coverage report skipped.
    goto after_cov
)
!PY! -m gcovr --version >nul 2>&1
if errorlevel 1 (
    echo  [WARNING] gcovr not installed. Run: !PY! -m pip install gcovr
    goto after_cov
)

rem gcovr runs from the parent folder (as06720): paths look like task_01/src/... and task_02/src/...
rem main.cpp (keyboard input) is excluded from the measurement.
pushd ..
!PY! -m gcovr -r . --filter ".*task_01/src/" --filter ".*task_02/src/" --exclude ".*main[.]cpp" --print-summary task_02/build > task_02\coverage_report.txt
!PY! -m gcovr -r . --filter ".*task_01/src/" --filter ".*task_02/src/" --exclude ".*main[.]cpp" --html-details task_02/coverage.html task_02/build
popd
type coverage_report.txt

:after_cov
if not "!RESULT!"=="0" goto error
echo.
echo  All tests passed! Reports: test_report.txt, test_report.xml, coverage_report.txt, coverage.html
echo.
pause
exit /b 0

:error
echo.
echo *** FAILED! See output above. ***
echo.
pause
exit /b 1
