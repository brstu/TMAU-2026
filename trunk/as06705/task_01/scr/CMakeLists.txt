cmake_minimum_required(VERSION 3.16)
project(task_01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task_01 main.cpp)
target_include_directories(task_01 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if (MSVC)
    target_compile_options(task_01 PRIVATE /utf-8 /W4)
else()
    target_compile_options(task_01 PRIVATE -Wall -Wextra)
endif()