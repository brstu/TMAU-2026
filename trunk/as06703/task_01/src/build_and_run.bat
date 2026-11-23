@echo off
setlocal EnableExtensions DisableDelayedExpansion

title Variant 4 - Build and Run

rem Resolve this BAT file directory without a trailing backslash.
for %%I in ("%~dp0.") do set "SOURCE_DIR=%%~fI"
set "BUILD_DIR=%SOURCE_DIR%\build"

echo ================================================================
echo                 VARIANT 4 - BUILD AND RUN
echo ================================================================
echo.
echo Source directory: "%SOURCE_DIR%"
echo Build directory:  "%BUILD_DIR%"
echo.

echo [1/3] Checking CMake...
where cmake >nul 2>nul
if errorlevel 1 goto cmake_error

if not exist "%SOURCE_DIR%\CMakeLists.txt" goto cmakelists_error
if not exist "%SOURCE_DIR%\main.cpp" goto source_error
if not exist "%SOURCE_DIR%\plot.py" goto plot_error

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if errorlevel 1 goto build_directory_error

echo.
echo [2/3] Configuring the project...
cmake -S "%SOURCE_DIR%" -B "%BUILD_DIR%"
if errorlevel 1 goto configure_error

echo.
echo [3/3] Building the project...
cmake --build "%BUILD_DIR%" --config Release --parallel
if errorlevel 1 goto build_error

set "PROGRAM=%BUILD_DIR%\Release\variant4.exe"
if not exist "%PROGRAM%" set "PROGRAM=%BUILD_DIR%\variant4.exe"
if not exist "%PROGRAM%" goto executable_error

echo.
echo ================================================================
echo                    STARTING PROGRAM
echo ================================================================
echo.

rem Run from src so CSV files and results_plot.png are saved beside plot.py.
pushd "%SOURCE_DIR%"
if errorlevel 1 goto directory_error
"%PROGRAM%"
set "APP_EXIT_CODE=%ERRORLEVEL%"
popd
if not "%APP_EXIT_CODE%"=="0" goto application_error

echo.
echo Program finished successfully.
pause
endlocal
exit /b 0

:cmake_error
echo ERROR: CMake was not found. Install CMake and add it to PATH.
goto fail

:cmakelists_error
echo ERROR: CMakeLists.txt was not found in the src folder.
goto fail

:source_error
echo ERROR: main.cpp was not found in the src folder.
goto fail

:plot_error
echo ERROR: plot.py was not found in the src folder.
goto fail

:build_directory_error
echo ERROR: Could not create the build folder.
goto fail

:configure_error
echo ERROR: CMake configuration failed.
goto fail

:build_error
echo ERROR: Compilation failed.
goto fail

:directory_error
echo ERROR: Could not open the src folder.
goto fail

:executable_error
echo ERROR: variant4.exe was not found after compilation.
goto fail

:application_error
echo ERROR: The program returned a non-zero exit code.
goto fail

:fail
echo.
pause
endlocal
exit /b 1
