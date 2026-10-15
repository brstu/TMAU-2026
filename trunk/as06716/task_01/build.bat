@echo off
chcp 65001 >nul
setlocal
cd /d "%~dp0"

echo ===== [1/3] Configuring project (CMake) =====
cmake -S . -B build
if errorlevel 1 goto :error

echo.
echo ===== [2/3] Building project (Release) =====
cmake --build build --config Release
if errorlevel 1 goto :error

echo.
echo ===== [3/3] Running program =====
build\Release\task_01_as06716.exe
if errorlevel 1 goto :error

echo.
echo ===== Plotting results (Python / Matplotlib) =====
python src\plot.py
if errorlevel 1 echo Plotting failed. Try: pip install matplotlib

pause
endlocal
exit /b 0

:error
echo.
echo ERROR: a step failed, exit code %errorlevel%.
pause
endlocal
exit /b 1
