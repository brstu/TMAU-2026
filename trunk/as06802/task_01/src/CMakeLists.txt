cmake_minimum_required(VERSION 3.10)

project(task01_Variant6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task_01_as06802
    main.cpp
    Model.h
    Models.h
)

set_target_properties(task_01_as06802 PROPERTIES
    OUTPUT_NAME task_01
)

if(MSVC)
    target_compile_options(task_01_as06802 PRIVATE /utf-8 /W4)
else()
    target_compile_options(task_01_as06802 PRIVATE -Wall -Wextra -Wpedantic)
endif()
