cmake_minimum_required(VERSION 3.10)

project(task01_variant10 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task_01
    RealizeModel.cpp
    models.h
)

if(MSVC)
    target_compile_options(task_01 PRIVATE /utf-8 /W4)
else()
    target_compile_options(task_01 PRIVATE -Wall -Wextra)
endif()