cmake_minimum_required(VERSION 3.20)

project(tmau_task_02_src LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(tmau_task_02_exe
    stability_analyzer.cpp
)