cmake_minimum_required(VERSION 3.16)
project(as06714_lab2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(AS06714_COVERAGE "Build with gcov instrumentation" OFF)
if(AS06714_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
