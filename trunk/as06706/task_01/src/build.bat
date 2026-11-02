@echo off
setlocal EnableExtensions
chcp 65001 >nul

set "ROOT=%~dp0."
set "BUILD_DIR=%ROOT%\out\build\x64-Debug"
set "BIN_DIR=%BUILD_DIR%\bin"

cd /d "%ROOT%"

echo ===============================================
echo           CMAKE PROJECT BUILD
echo ===============================================
echo.

if not exist "%ROOT%\CMakeLists.txt" goto no_project

echo [1/3] Checking CMake...
where cmake >nul 2>&1
if errorlevel 1 goto no_cmake

echo.
echo [2/3] Configuring project...
cmake -S "%ROOT%" -B "%BUILD_DIR%"
if errorlevel 1 goto failed

echo.
echo [3/3] Building project...
cmake --build "%BUILD_DIR%" --config Debug
if errorlevel 1 goto failed

echo.
echo BUILD COMPLETED.
echo.

if not exist "%BIN_DIR%" goto no_executable

set "APP="
for %%F in ("%BIN_DIR%\*.exe") do (
    if exist "%%~fF" set "APP=%%~fF"
)

if not defined APP goto no_executable

echo Starting application...
echo.

pushd "%BIN_DIR%"
"%APP%"
set "APP_RESULT=%ERRORLEVEL%"
popd

echo.
echo APPLICATION FINISHED.
pause
exit /b %APP_RESULT%

:no_project
echo ERROR: CMakeLists.txt was not found in the project folder.
goto failed

:no_cmake
echo ERROR: CMake was not found in PATH.
echo Install CMake and try again.
goto failed

:no_executable
echo ERROR: Executable was not found in the bin folder.
echo Check the executable output directory in CMakeLists.txt.
goto failed

:failed
echo.
echo BUILD OR LAUNCH FAILED.
pause
exit /b 1