@echo off
cd /d "%~dp0"

echo  CMake configure 
cmake -S . -B build
if errorlevel 1 goto fail

echo.
echo  Build 
cmake --build build --config Release
if errorlevel 1 goto fail

echo.
echo  Run 
if exist build\Release\as06816_task_01.exe (
    build\Release\as06816_task_01.exe
) else (
    build\as06816_task_01.exe
)

echo.
echo Build finished. Graphs: python plot.py
pause
exit /b 0

:fail
echo.
echo Build failed!
pause
exit /b 1
