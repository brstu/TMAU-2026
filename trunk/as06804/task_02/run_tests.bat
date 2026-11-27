@echo off
setlocal EnableDelayedExpansion
set "PROJECT_ROOT=%~dp0"
cd /d "%PROJECT_ROOT%"

echo =============================================
echo  TMAU lab 2 (as06804): build + tests + coverage
echo =============================================

rem --- Toolchain -----------------------------------------------------------
set "MINGW_BIN=C:\mingw32\bin"
if defined MINGW_BIN_OVERRIDE set "MINGW_BIN=%MINGW_BIN_OVERRIDE%"
set "PATH=%MINGW_BIN%;%PATH%"

set "CMAKE_EXE=%MINGW_BIN%\cmake.exe"
if not exist "!CMAKE_EXE!" (
    where cmake >nul 2>nul && set "CMAKE_EXE=cmake"
)
if not defined CMAKE_EXE (
    echo ERROR: cmake.exe not found. Set MINGW_BIN_OVERRIDE.
    pause
    exit /b 1
)

set "MAKE_EXE=%MINGW_BIN%\mingw32-make.exe"
if not exist "!MAKE_EXE!" set "MAKE_EXE=mingw32-make"
set "GCOV_EXE=%MINGW_BIN%\gcov.exe"
if not exist "!GCOV_EXE!" set "GCOV_EXE=gcov"

echo [OK] CMake : !CMAKE_EXE!
echo [OK] Make  : !MAKE_EXE!
echo [OK] gcov  : !GCOV_EXE!
echo.

rem --- Google Test source (MinGW CMake has no CA roots, use Python download) --
set "GTEST_ROOT=%TEMP%\tmau_gtest_cache\googletest-src"
if not exist "!GTEST_ROOT!\CMakeLists.txt" (
    echo Downloading Google Test v1.14.0...
    set "TMAU_GTEST_ROOT=!GTEST_ROOT!"
    python -c "import io, os, urllib.request, zipfile, shutil; d=os.environ['TMAU_GTEST_ROOT']; p=os.path.dirname(d); os.makedirs(p, exist_ok=True); shutil.rmtree(d, ignore_errors=True); b=urllib.request.urlopen('https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip', timeout=60).read(); z=zipfile.ZipFile(io.BytesIO(b)); z.extractall(p); n=[x for x in os.listdir(p) if x.startswith('googletest-') and os.path.isdir(os.path.join(p,x))]; os.rename(os.path.join(p,n[0]), d) if n else None"
    if not exist "!GTEST_ROOT!\CMakeLists.txt" (
        echo ERROR: could not prepare Google Test sources. Check internet access.
        pause
        exit /b 1
    )
)
echo [OK] GTest : !GTEST_ROOT!

rem --- ASCII working mirror (MinGW make cannot handle Cyrillic paths) ------
set "WORK_ROOT=%TEMP%\tmau_task02_build"
if exist "!WORK_ROOT!" rmdir /s /q "!WORK_ROOT!"
mkdir "!WORK_ROOT!\src" "!WORK_ROOT!\test"
xcopy /E /I /Y /Q "src\*" "!WORK_ROOT!\src\" >nul
xcopy /E /I /Y /Q "test\*" "!WORK_ROOT!\test\" >nul
echo [OK] Work  : !WORK_ROOT!
echo.
pushd "!WORK_ROOT!"

rem --- Configure -----------------------------------------------------------
echo [1/4] Configuring...
"!CMAKE_EXE!" -S src -B build -G "MinGW Makefiles" ^
    -DCMAKE_MAKE_PROGRAM="!MAKE_EXE!" ^
    -DCMAKE_C_COMPILER="%MINGW_BIN%\gcc.exe" ^
    -DCMAKE_CXX_COMPILER="%MINGW_BIN%\g++.exe" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DTASK02_BUILD_TESTS=ON ^
    -DTASK02_ENABLE_COVERAGE=ON ^
    -DFETCHCONTENT_BASE_DIR="%TEMP%\tmau_gtest_cache\internal" ^
    -DFETCHCONTENT_SOURCE_DIR_GOOGLETEST="!GTEST_ROOT!"
if !ERRORLEVEL! NEQ 0 (
    echo ERROR: CMake configure failed.
    popd
    pause
    exit /b 1
)

rem --- Build ---------------------------------------------------------------
echo.
echo [2/4] Building...
"!CMAKE_EXE!" --build build --parallel
if !ERRORLEVEL! NEQ 0 (
    echo ERROR: build failed.
    popd
    pause
    exit /b 1
)

rem --- Tests ---------------------------------------------------------------
echo.
echo [3/4] Running Google Test...
set "TEST_EXE=build\task02_tests.exe"
if not exist "!TEST_EXE!" set "TEST_EXE=build\Debug\task02_tests.exe"
if not exist "!TEST_EXE!" (
    echo ERROR: task02_tests.exe not found.
    popd
    pause
    exit /b 1
)

"!TEST_EXE!"
set "TEST_RESULT=!ERRORLEVEL!"

rem --- Coverage ------------------------------------------------------------
echo.
echo [4/4] Coverage...
set "GCOV_ARG=!GCOV_EXE:\=/!"
python -m gcovr --version >nul 2>nul
if !ERRORLEVEL! NEQ 0 (
    echo ERROR: gcovr is not installed. Run: python -m pip install --user gcovr
    popd
    pause
    exit /b 1
)

rem Testable core only; interactive main.cpp is excluded.
python -m gcovr --root . --filter src --exclude src/main.cpp ^
    --gcov-executable "!GCOV_ARG!" ^
    build
if !ERRORLEVEL! NEQ 0 (
    echo ERROR: gcovr failed.
    popd
    pause
    exit /b 1
)

popd

echo.
echo =============================================
if !TEST_RESULT! EQU 0 (
    echo SUCCESS: all tests passed.
) else (
    echo FAILURE: some tests failed.
)
echo =============================================
pause
endlocal
exit /b !TEST_RESULT!
