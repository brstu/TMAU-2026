cmake_minimum_required(VERSION 3.15)
project(task_01_as06716 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_executable(task_01_as06716 src/main.cpp src/models.hpp)

target_compile_definitions(task_01_as06716 PRIVATE DOC_DIR="${CMAKE_SOURCE_DIR}/doc")
