@echo off
setlocal EnableExtensions
chcp 65001 >nul
pushd "%~dp0"
if errorlevel 1 exit /b 1

echo ============================================================
echo TIMAY PROJECT BUILD - VARIANT 14
echo ============================================================
echo.

where cmake >nul 2>nul
if errorlevel 1 goto :no_cmake

set "TIMAY_PYTHON="
if defined LOCALAPPDATA (
    for %%V in (314 313 312 311 310 39) do (
        if not defined TIMAY_PYTHON if exist "%LOCALAPPDATA%\Programs\Python\Python%%V\python.exe" set "TIMAY_PYTHON=%LOCALAPPDATA%\Programs\Python\Python%%V\python.exe"
    )
)

if not defined TIMAY_PYTHON (
    for /f "delims=" %%P in ('where python.exe 2^>nul ^| findstr /i /v "WindowsApps"') do (
        if not defined TIMAY_PYTHON set "TIMAY_PYTHON=%%P"
    )
)

if not defined TIMAY_PYTHON (
    where py.exe >nul 2>nul
    if not errorlevel 1 set "TIMAY_PYTHON=py -3"
)

if defined TIMAY_PYTHON (
    echo Python found: %TIMAY_PYTHON%
) else (
    echo WARNING: Python was not found. The simulation can run, but the graph cannot be generated.
    echo Install Python 3 and Matplotlib, or add Python to PATH.
)

echo.
echo ------------------- CMAKE CONFIGURATION -------------------
cmake -S . -B build
if errorlevel 1 goto :build_failed

echo.
echo ------------------- COMPILATION ---------------------------
cmake --build build --config Release --clean-first --parallel
if errorlevel 1 goto :build_failed

echo.
echo Build completed successfully.
echo Starting simulation...
echo.

set "APP="
if exist "build\Release\TimayVariant14.exe" set "APP=build\Release\TimayVariant14.exe"
if exist "build\TimayVariant14.exe" set "APP=build\TimayVariant14.exe"
if not defined APP goto :no_exe

"%APP%"
set "APP_EXIT=%ERRORLEVEL%"
echo.
echo Simulation program finished with code %APP_EXIT%.
popd
pause
exit /b %APP_EXIT%

:no_cmake
echo ERROR: CMake was not found. Install CMake and add it to PATH.
goto :failed

:no_exe
echo ERROR: TimayVariant14.exe was not found in the build folder.
goto :failed

:build_failed
echo ERROR: CMake configuration or compilation failed.
goto :failed

:failed
echo.
echo Build or launch failed. Check the messages above.
popd
pause
exit /b 1
