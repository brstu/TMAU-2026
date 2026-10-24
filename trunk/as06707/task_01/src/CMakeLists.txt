cmake_minimum_required(VERSION 3.10)
project(ModelSimulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tmau_task_01
    main.cpp
    Model1_1.cpp
    Model2_5.cpp
    Model3_9.cpp
)