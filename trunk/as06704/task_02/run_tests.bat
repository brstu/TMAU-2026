@echo off
setlocal EnableExtensions
chcp 65001 >nul
title TiMAY Variant 12 - Build, Tests and Coverage

cd /d "%~dp0"
set "PROJECT_DIR=%CD%"
set "BUILD_DIR=%PROJECT_DIR%\build"
set "SOURCE_DIR=%PROJECT_DIR%\src"
set "TEST_REPORT=%PROJECT_DIR%\test_report.txt"
set "COVERAGE_LOG=%PROJECT_DIR%\coverage_run.txt"
set "HTML_DIR=%PROJECT_DIR%\coverage_html"
set "XML_FILE=%PROJECT_DIR%\coverage.xml"
set "SUMMARY_FILE=%PROJECT_DIR%\coverage_summary.txt"
set "OPENCPP_EXE=%ProgramFiles%\OpenCppCoverage\OpenCppCoverage.exe"

if not exist "%OPENCPP_EXE%" (
    for /f "delims=" %%I in ('where OpenCppCoverage.exe 2^>nul') do (
        if not defined OPENCPP_FOUND set "OPENCPP_FOUND=%%I"
    )
    if defined OPENCPP_FOUND set "OPENCPP_EXE=%OPENCPP_FOUND%"
)

echo ==============================================
echo TiMAY Variant 12 - Build, Tests and Coverage
echo ==============================================
echo.

if not exist "%PROJECT_DIR%\CMakeLists.txt" (
    echo ERROR: CMakeLists.txt was not found.
    goto :failed
)

if not exist "%SOURCE_DIR%\main.cpp" (
    echo ERROR: src\main.cpp was not found.
    goto :failed
)

where cmake >nul 2>nul
if errorlevel 1 (
    echo ERROR: CMake was not found in PATH.
    goto :failed
)

if not exist "%OPENCPP_EXE%" (
    echo ERROR: OpenCppCoverage.exe was not found.
    echo Expected path: %ProgramFiles%\OpenCppCoverage\OpenCppCoverage.exe
    goto :failed
)

echo OpenCppCoverage found:
echo %OPENCPP_EXE%
echo.

echo Configuring project with CMake...
cmake -S "%PROJECT_DIR%" -B "%BUILD_DIR%" -DBUILD_TESTING=ON -DENABLE_COVERAGE=OFF
if errorlevel 1 goto :cmake_failed

echo.
echo Building application and Google Test suite...
cmake --build "%BUILD_DIR%" --config Debug --parallel
if errorlevel 1 goto :build_failed

set "TEST_EXE=%BUILD_DIR%\test\TiMAY_Tests.exe"
if not exist "%TEST_EXE%" set "TEST_EXE=%BUILD_DIR%\test\Debug\TiMAY_Tests.exe"
if not exist "%TEST_EXE%" set "TEST_EXE=%BUILD_DIR%\test\Release\TiMAY_Tests.exe"
if not exist "%TEST_EXE%" (
    echo ERROR: TiMAY_Tests.exe was not found.
    goto :failed
)

echo.
echo Running unit tests with CTest...
ctest --test-dir "%BUILD_DIR%" -C Debug --output-on-failure > "%TEST_REPORT%" 2>&1
set "TEST_RESULT=%ERRORLEVEL%"
type "%TEST_REPORT%"
if not "%TEST_RESULT%"=="0" goto :tests_failed

if exist "%HTML_DIR%" rmdir /s /q "%HTML_DIR%"
if exist "%XML_FILE%" del /q "%XML_FILE%"
if exist "%COVERAGE_LOG%" del /q "%COVERAGE_LOG%"
if exist "%SUMMARY_FILE%" del /q "%SUMMARY_FILE%"

echo.
echo Running test executable under OpenCppCoverage...
"%OPENCPP_EXE%" --sources "%SOURCE_DIR%" --export_type=html:"%HTML_DIR%" --export_type=cobertura:"%XML_FILE%" -- "%TEST_EXE%" --gtest_color=no > "%COVERAGE_LOG%" 2>&1
set "COVERAGE_RESULT=%ERRORLEVEL%"
type "%COVERAGE_LOG%"
if not "%COVERAGE_RESULT%"=="0" goto :coverage_failed
if not exist "%HTML_DIR%\index.html" goto :coverage_failed
if not exist "%XML_FILE%" goto :coverage_failed

echo.
echo Calculating line coverage summary...
powershell -NoProfile -ExecutionPolicy Bypass -Command "$doc = New-Object System.Xml.XmlDocument; $doc.Load($env:XML_FILE); $rate = [double]$doc.DocumentElement.GetAttribute('line-rate'); $percent = 100 * $rate; $summary = 'Line coverage: {0:N2} percent' -f $percent; Set-Content -LiteralPath $env:SUMMARY_FILE -Value $summary -Encoding ASCII; Write-Output $summary"
if errorlevel 1 goto :summary_failed
if not exist "%SUMMARY_FILE%" goto :summary_failed

echo.
echo ==============================================
echo ALL UNIT TESTS AND COVERAGE COMPLETED
echo ==============================================
echo.
echo Test report: test_report.txt
echo Coverage report: coverage_html\index.html
echo Coverage XML: coverage.xml
echo Coverage summary: coverage_summary.txt
echo.
start "" "%HTML_DIR%\index.html"
pause >nul
exit /b 0

:cmake_failed
echo ERROR: CMake configuration failed.
goto :failed

:build_failed
echo ERROR: Project compilation failed.
goto :failed

:tests_failed
echo ERROR: One or more unit tests failed. See test_report.txt.
goto :failed

:coverage_failed
echo ERROR: OpenCppCoverage failed. See coverage_run.txt.
goto :failed

:summary_failed
echo ERROR: Could not create coverage_summary.txt from coverage.xml.
goto :failed

:failed
echo.
echo BUILD, TEST OR COVERAGE STEP FAILED.
echo Check the messages and report files above.
echo.
pause
exit /b 1
