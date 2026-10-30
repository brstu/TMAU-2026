
cmake_minimum_required(VERSION 3.20)

project(task_01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task_01 main.cpp)

if(MSVC)
    target_compile_options(task_01 PRIVATE /utf-8)
endif()
