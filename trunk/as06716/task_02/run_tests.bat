@echo off
setlocal
cd /d "%~dp0"
if not exist report mkdir report

echo === [1/3] Build (Visual Studio) ===
cmake -S . -B build || goto :error
cmake --build build --config Debug || goto :error

echo.
echo === [2/3] Run tests (ctest) ===
pushd build
ctest -C Debug --output-on-failure --output-junit ..\report\junit.xml > ..\report\tests.txt 2>&1
set TESTS_RESULT=%errorlevel%
popd
type report\tests.txt

echo.
echo === [3/3] Code coverage (MinGW + gcovr) ===
set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist build_cov rmdir /s /q build_cov
cmake -S . -B build_cov -G Ninja -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ || goto :error
cmake --build build_cov || goto :error
pushd build_cov
ctest --output-on-failure > ..\report\tests_coverage_build.txt 2>&1
popd
gcovr -r . build_cov --filter "src/" --exclude "src/main.cpp" --txt report\coverage.txt --html-details report\coverage.html --print-summary
type report\coverage.txt

echo.
if "%TESTS_RESULT%"=="0" (echo RESULT: ALL TESTS PASSED) else (echo RESULT: SOME TESTS FAILED, see report\tests.txt)
echo Reports: report\tests.txt, report\junit.xml, report\coverage.txt, report\coverage.html
pause
exit /b %TESTS_RESULT%

:error
echo.
echo ERROR: a step failed.
pause
exit /b 1