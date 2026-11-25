cmake_minimum_required(VERSION 3.15)
project(task_01_as06716 LANGUAGES CXX)

add_executable(task_01_as06716 main.cpp models.hpp)
target_compile_features(task_01_as06716 PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(task_01_as06716 PRIVATE /W4 /utf-8)
else()
    target_compile_options(task_01_as06716 PRIVATE -Wall -Wextra)
endif()

target_compile_definitions(task_01_as06716 PRIVATE DOC_DIR="${CMAKE_CURRENT_SOURCE_DIR}/../doc")
