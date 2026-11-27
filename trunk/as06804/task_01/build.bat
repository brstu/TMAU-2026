@echo off
echo Starting build process...

:: Жестко прописываем пути к твоим файлам (как мы делали ранее)
set CMAKE_EXE="C:\mingw32\bin\cmake.exe"
set MAKE_EXE="C:\mingw32\bin\mingw32-make.exe"
set CXX_COMPILER="C:\mingw32\bin\g++.exe"
set C_COMPILER="C:\mingw32\bin\gcc.exe"

:: Добавляем папку bin в PATH
set PATH=C:\mingw32\bin;%PATH%

if not exist build mkdir build
cd build

echo Running CMake...
%CMAKE_EXE% -G "MinGW Makefiles" -DCMAKE_MAKE_PROGRAM=%MAKE_EXE% -DCMAKE_C_COMPILER=%C_COMPILER% -DCMAKE_CXX_COMPILER=%CXX_COMPILER% ..
if %errorlevel% neq 0 (
    echo CMake failed!
    pause
    exit /b %errorlevel%
)

echo Building project...
%CMAKE_EXE% --build .
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b %errorlevel%
)

echo Build successful!
cd ..

:: Запускаем саму программу (она ждет ввода количества шагов)
echo Running program...
build\lab1_app.exe

:: Как только программа завершится, запускаем скрипт визуализации
echo Generating plots...
python plot_results.py

pause