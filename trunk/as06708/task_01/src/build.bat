@echo off
chcp 65001 > 0
setlocal EnableDelayedExpansion

if not exist "%~dp0/CMakeLists.txt" (
	echo ОШИБКА CMakeLists не был найден
	pause
	exit /b 1
)

echo НАЧАЛО СБОРКИ ПРОГРАММЫ

echo ЭТАП 1: Конфигурация
cmake -S . -B build
if errorlevel 1 goto error

echo ЭТАП 2: Компиляция Release сборки
cmake --build build --config Release
if errorlevel 1 goto error

if not exist "%~dp0src/plot.py" (
	echo ОШИБКА plot.py не был найден в папке src/
	pause
	exit /b 1
)

copy /Y "%~dp0src\plot.py" "%~dp0build\Release"

if not exist "%~dp0build/Release/plot.py" (
	echo Внимание!: plot.py не был скопирован в build/Release/
	echo Переместите файл plot.py собственными силами в build/Release/
)

echo УСПЕХ: Сборка произошла успешно. Исполняемый файл находиться в "build/Release/"
echo P.S.
echo Для корректной работы программы необходимо иметь в точке вызова программы файл plot.py
goto end 

:error
echo ПРИ СБОРКЕ ПРОЕКТА ПРОИЗОШЛА ОШИБКА
exit /b 1

:end
endlocal