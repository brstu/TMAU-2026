cmake_minimum_required(VERSION 3.15)
project(TMAU_Variant19 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include coverage instrumentation when requested by run_tests.bat
option(ENABLE_COVERAGE "Build with gcov coverage instrumentation" OFF)
if(ENABLE_COVERAGE)
    add_compile_options(-O0 -g --coverage)
    add_link_options(--coverage)
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
