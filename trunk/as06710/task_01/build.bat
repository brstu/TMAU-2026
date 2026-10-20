@echo off
setlocal
cd /d "%~dp0"

if not exist CMakeLists.txt (
    echo ERROR: CMakeLists.txt not found
    pause
    exit /b 1
)

echo ===== STARTING BUILD PROCESS =====
echo [STAGE 1/3] Configuration
cmake -S . -B build -G "MinGW Makefiles" ^
      -DCMAKE_BUILD_TYPE=Release ^
      -DCMAKE_C_COMPILER=gcc ^
      -DCMAKE_CXX_COMPILER=g++ ^
      -DCMAKE_MAKE_PROGRAM=mingw32-make
if errorlevel 1 goto error

echo [STAGE 2/3] Compilation
cmake --build build
if errorlevel 1 goto error

echo [STAGE 3/3] Launch
if exist build\src\tmau_task_01.exe (
    cd build\src
) else (
    cd build
)
tmau_task_01.exe
echo ===== DONE =====
pause
goto end

:error
echo ERROR: Build failed
pause
exit /b 1

:end
endlocal