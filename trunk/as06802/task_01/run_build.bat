@echo off
chcp 65001 >nul
echo =======================================
echo Building project via CMake...
echo =======================================

cmake -S . -B build
cmake --build build --config Release

if %errorlevel% neq 0 (
    echo.
    echo =======================================
    echo BUILD FAILED! Check the errors above.
    echo =======================================
    pause
    exit /b %errorlevel%
)

echo.
echo =======================================
echo Build successful! Running program...
echo =======================================

if exist build\Release\lab_app.exe (
    build\Release\lab_app.exe
) else (
    build\lab_app.exe
)

pause