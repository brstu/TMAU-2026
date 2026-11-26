@echo off
rem ==== Lab 2, as06714, variant 7: build + unit tests + coverage ====
setlocal
pushd "%~dp0"

set BUILD_DIR=build_tests
set EXIT_CODE=0

where cmake >nul 2>nul || (
    echo cmake.exe is not in PATH. Install CMake and reopen the console.
    set EXIT_CODE=2
    goto finish
)

rem --- pick a generator: GCC toolchain (coverage possible) or the CMake default ---
set GEN_ARGS=
set WITH_COV=OFF
where g++ >nul 2>nul && (
    where ninja >nul 2>nul && set GEN_ARGS=-G Ninja
    if not defined GEN_ARGS where mingw32-make >nul 2>nul && set GEN_ARGS=-G "MinGW Makefiles"
)
if defined GEN_ARGS set WITH_COV=ON

echo.
echo ---- 1. configure (coverage: %WITH_COV%)
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
cmake -S . -B %BUILD_DIR% %GEN_ARGS% -DCMAKE_BUILD_TYPE=Debug -DAS06714_COVERAGE=%WITH_COV%
if errorlevel 1 (set EXIT_CODE=1& goto finish)

echo.
echo ---- 2. build
cmake --build %BUILD_DIR% --config Debug
if errorlevel 1 (set EXIT_CODE=1& goto finish)

echo.
echo ---- 3. tests
ctest --test-dir %BUILD_DIR% -C Debug --output-on-failure --output-junit "%CD%\tests_junit.xml" > tests_log.txt 2>&1
if errorlevel 1 set EXIT_CODE=1
type tests_log.txt

echo.
echo ---- 4. coverage
if "%WITH_COV%"=="OFF" (
    echo skipped: compiler is not GCC
    goto finish
)
python -m gcovr --version >nul 2>nul || (
    echo skipped: run "python -m pip install gcovr" first
    goto finish
)
if not exist coverage mkdir coverage
rem paths are measured from the student folder so that task_01 sources are included
python -m gcovr -r .. --filter ".*task_01/src/" --filter ".*task_02/src/" --exclude ".*main[.]cpp" --print-summary %BUILD_DIR% > coverage_summary.txt
python -m gcovr -r .. --filter ".*task_01/src/" --filter ".*task_02/src/" --exclude ".*main[.]cpp" --html-details coverage\index.html %BUILD_DIR%
type coverage_summary.txt

:finish
echo.
if "%EXIT_CODE%"=="0" (echo RESULT: OK) else (echo RESULT: FAILED, code %EXIT_CODE%)
popd
pause
exit /b %EXIT_CODE%
