@echo off
echo ============================================
echo  Building Task 01 (Variant 12)
echo ============================================

if not exist build (
    mkdir build
)

cd build

echo.
echo [1/3] Running CMake configure...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=mingw32-make
if %ERRORLEVEL% NEQ 0 (
    echo CMake configure failed!
    cd ..
    exit /b %ERRORLEVEL%
)

echo.
echo [2/3] Building...
cmake --build . --config Release
if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    cd ..
    exit /b %ERRORLEVEL%
)

echo.
echo [3/3] Running demo...
task01.exe
if %ERRORLEVEL% NEQ 0 (
    echo Program exited with error code %ERRORLEVEL%
)

echo.
echo Build and run completed successfully.
cd ..
pause
