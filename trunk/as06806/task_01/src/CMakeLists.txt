cmake_minimum_required(VERSION 3.10)

project(task01_Variant6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task_01
    main.cpp
    model.h
)

if(MSVC)
    target_compile_options(task_01 PRIVATE /utf-8)
endif()