@echo off
setlocal
cd /d "%~dp0"
echo === Configure CMake ===
cmake -S . -B build
if errorlevel 1 goto error
echo === Build project ===
cmake --build build --config Release
if errorlevel 1 goto error
echo === Run simulation ===
if exist "build\Release\task01.exe" (
  "build\Release\task01.exe"
) else (
  "build\task01.exe"
)
if errorlevel 1 goto error
echo === Plot results ===
python src\plot.py results.csv
if errorlevel 1 echo Не удалось открыть график. Проверьте установку Python и Matplotlib.
pause
exit /b 0
:error
echo Сборка или запуск завершились с ошибкой.
pause
exit /b 1
