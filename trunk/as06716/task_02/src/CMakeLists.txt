cmake_minimum_required(VERSION 3.15)
project(task_02_as06716 LANGUAGES CXX)

option(ENABLE_COVERAGE "Instrument code for gcov/gcovr (gcc/clang only)" OFF)

add_library(task02_core STATIC
    app.cpp
    models.cpp
    console.cpp
    results.cpp
    signals.cpp
    simulator.cpp
    stability.cpp
    app.hpp
    console.hpp
    models.hpp
    results.hpp
    signals.hpp
    simulator.hpp
    stability.hpp)
target_include_directories(task02_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(task02_core PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(task02_core PUBLIC /W4 /utf-8)
else()
    target_compile_options(task02_core PUBLIC -Wall -Wextra)
endif()

if(ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(task02_core PUBLIC --coverage -O0 -g)
    target_link_options(task02_core PUBLIC --coverage)
endif()

add_executable(task_02_as06716 main.cpp)
target_link_libraries(task_02_as06716 PRIVATE task02_core)
target_compile_definitions(task_02_as06716 PRIVATE DOC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../doc")
