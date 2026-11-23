@echo off
setlocal EnableExtensions DisableDelayedExpansion
pushd "%~dp0" || exit /b 1
if not exist build mkdir build
set "stage=Starting"
>build\run-status.log echo Started: %date% %time%
echo Project: %CD%

set "cmake=cmake.exe"
set "ctest=ctest.exe"
set "build_dir=%CD%\build\msvc"
set "generator="
set "vs_path="

where g++.exe >nul 2>&1 && where mingw32-make.exe >nul 2>&1 && set "generator=-G "MinGW Makefiles""
if defined generator set "build_dir=%CD%\build\mingw"
if not defined generator call :find_vs

where cmake.exe >nul 2>&1
if errorlevel 1 (
    call :use_vs_cmake || goto missing_cmake
)
if not defined generator if defined vs_path (
    call :use_vs_ninja || goto failed
)

set "stage=Configuring and building"
>>build\run-status.log echo [1/4] Configuring and building
echo [1/4] Configuring and building Lab 03...
"%cmake%" -S . -B "%build_dir%" %generator% -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON || goto failed
"%cmake%" --build "%build_dir%" --config Debug || goto failed

set "stage=Running tests"
>>build\run-status.log echo [2/4] Running tests
echo [2/4] Running Google Test...
"%ctest%" --test-dir "%build_dir%" -C Debug --no-tests=error --output-on-failure --output-junit "%build_dir%\test-results.xml" || goto failed
echo Test report: "%build_dir%\test-results.xml"

set "stage=Generating documentation"
>>build\run-status.log echo [3/4] Generating documentation
echo [3/4] Generating Doxygen documentation...
set "docs_status=0"
if not defined DOXYGEN_EXE if exist "%CD%\build\dependencies\doxygen\doxygen.exe" (
    set "DOXYGEN_EXE=%CD%\build\dependencies\doxygen\doxygen.exe"
)
if not defined DOXYGEN_EXE (
    where doxygen.exe >nul 2>&1 && set "DOXYGEN_EXE=doxygen.exe"
)
if not defined DOXYGEN_EXE if exist "%ProgramFiles%\doxygen\bin\doxygen.exe" (
    set "DOXYGEN_EXE=%ProgramFiles%\doxygen\bin\doxygen.exe"
)
if not defined DOXYGEN_EXE (
    echo Doxygen was not found. Install it or set DOXYGEN_EXE.
    echo The bundled HTML documentation remains available in doc\html.
    set "docs_status=1"
) else (
    "%DOXYGEN_EXE%" Doxyfile
    if errorlevel 1 (
        echo Doxygen failed. Continuing to the plots.
        set "docs_status=1"
    ) else (
        echo API documentation: "%CD%\doc\html\index.html"
    )
)

set "stage=Simulating and opening plots"
>>build\run-status.log echo [4/4] Simulating and opening plots
echo [4/4] Simulating examples and opening the plots...
set "app=%build_dir%\src\task03.exe"
if not exist "%app%" set "app=%build_dir%\src\Debug\task03.exe"
"%app%" --examples || goto failed

set "python_args="
if not defined LAB03_PYTHON if exist "%CD%\build\dependencies\python\python.exe" (
    set "LAB03_PYTHON=%CD%\build\dependencies\python\python.exe"
)
if not defined LAB03_PYTHON (
    where py.exe >nul 2>&1 && set "LAB03_PYTHON=py.exe" && set "python_args=-3"
)
if not defined LAB03_PYTHON (
    where python.exe >nul 2>&1 && set "LAB03_PYTHON=python.exe"
)
if not defined LAB03_PYTHON goto saved_plots
"%LAB03_PYTHON%" %python_args% -c "import matplotlib" >nul 2>&1 || goto saved_plots

set "MPLCONFIGDIR=%build_dir%\matplotlib-cache"
set "figure_dir=%build_dir%\figures"
"%LAB03_PYTHON%" %python_args% src\plot_results.py --examples --output-dir "%figure_dir%" || goto failed
echo New PNG plots: "%figure_dir%"
"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -STA -ExecutionPolicy Bypass -File "%CD%\src\show_plots.ps1" -FiguresDirectory "%figure_dir%" || goto failed
goto done

:saved_plots
echo Python with Matplotlib is unavailable. Opening the saved report figures.
echo Install Matplotlib with "py -m pip install matplotlib" to redraw the plots.
"%SystemRoot%\System32\WindowsPowerShell\v1.0\powershell.exe" -NoProfile -STA -ExecutionPolicy Bypass -File "%CD%\src\show_plots.ps1" -FiguresDirectory "%CD%\doc\figures" -Saved || goto failed

:done
popd
exit /b %docs_status%

:find_vs
set "vswhere=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%vswhere%" exit /b 1
for /f "usebackq delims=" %%I in (`"%vswhere%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.CMake.Project -property installationPath`) do set "vs_path=%%I"
if not defined vs_path exit /b 1
exit /b 0

:use_vs_cmake
if not defined vs_path call :find_vs || exit /b 1
set "cmake=%vs_path%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "ctest=%vs_path%\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\ctest.exe"
if not exist "%cmake%" exit /b 1
exit /b 0

:use_vs_ninja
set "vs_ninja=%vs_path%\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
if not exist "%vs_ninja%\ninja.exe" exit /b 0
if not exist "%vs_path%\VC\Auxiliary\Build\vcvars64.bat" exit /b 0
set "VSLANG=1033"
call "%vs_path%\VC\Auxiliary\Build\vcvars64.bat" || exit /b 1
set "PATH=%vs_ninja%;%PATH%"
set "generator=-G Ninja"
exit /b 0

:missing_cmake
echo CMake 3.21 or later is required.
:failed
echo [ERROR] Stage: %stage%
>>build\run-status.log echo FAILED: %stage%
echo Please send the error text above.
pause
popd
exit /b 1
