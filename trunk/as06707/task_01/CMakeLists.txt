cmake_minimum_required(VERSION 3.10)
project(ModelSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)

add_executable(tmau_task_01
    src/main.cpp
    src/Model1_1.cpp
    src/Model2_5.cpp
    src/Model3_9.cpp
)