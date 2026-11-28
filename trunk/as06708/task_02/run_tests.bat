@echo off 
 setlocal 
 cd /d "%~dp0" 
 if not exist report mkdir report 
  
 echo === [1/3] Build === 
 cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ || goto :error 
 cmake --build build || goto :error 
  
 echo. 
 echo === [2/3] Run tests (ctest) === 
 cd build 
 ctest -C Debug --output-on-failure --output-junit ..\report\junit.xml > ..\report\tests.txt 2>&1
 if errorlevel 1 (
    echo You have broken tests
    exit /b 1
 )
 set TESTS_RESULT=%errorlevel% 
 cd .. 
 type report\tests.txt 
  
 echo. 
 echo === [3/3] Code coverage gcovr === 
 if exist build_cov rmdir /s /q build_cov 
 cmake -S . -B build_cov -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=gcc -DENABLE_COVERAGE=OFF -DCMAKE_CXX_COMPILER=g++ || goto :error 
 cmake --build build_cov || goto :error 
 cd build_cov
 ctest -C Debug --output-on-failure > ..\report\tests_coverage_build.txt 2>&1 --output-junit build_cov/test_report.xml
 cd ..
 gcovr -r . --filter src/ --object-directory build_cov --exclude src/main.cpp --txt report\coverage.txt --html-details report\coverage.html --print-summary 
  
 echo. 
 if "%TESTS_RESULT%"=="0" (echo RESULT: ALL TESTS PASSED) else (echo RESULT: SOME TESTS FAILED, see report\tests.txt) 
 echo Reports: report\tests.txt, report\junit.xml, report\coverage.txt, report\coverage.html 
 pause 
 exit /b %TESTS_RESULT% 
 :error 
 echo. 
 echo ERROR: a step failed. 
 pause 
 exit /b 1