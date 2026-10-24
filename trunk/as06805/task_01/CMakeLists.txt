cmake_minimum_required(VERSION 3.15)
project(Task01_Variant3 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task_01
    src/main.cpp
)

target_include_directories(task_01 PRIVATE src)

if (MSVC)
    target_compile_options(task_01 PRIVATE /W4)
else()
    target_compile_options(task_01 PRIVATE -Wall -Wextra -O2)
endif()