@echo off
echo =======================================
echo  Cleaning build directory...
echo =======================================
if exist build rmdir /s /q build
mkdir build
cd build

echo =======================================
echo  Compiling project and running tests...
echo =======================================
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
if %ERRORLEVEL% NEQ 0 goto :error

cmake --build .
if %ERRORLEVEL% NEQ 0 goto :error

echo =======================================
echo  Running Google Test...
echo =======================================
ctest --output-on-failure
if %ERRORLEVEL% NEQ 0 goto :error

echo =======================================
echo  Generating Coverage Report (gcovr)
echo =======================================
REM Генерация отчета, исключая библиотеки, тесты и main.cpp
gcovr -r .. -e ".*_depts.*" -e ".*test.*" -e ".*main.cpp.*" --txt > coverage_report.txt
type coverage_report.txt

cd ..
echo.
echo =======================================
echo  SUCCESS! All tests passed.
echo =======================================
pause
exit /b 0

:error
echo.
echo =======================================
echo  FAILED! Check the errors above.
echo =======================================
cd ..
pause
exit /b 1