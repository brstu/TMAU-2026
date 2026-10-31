cmake_minimum_required(VERSION 3.10)
project(ControlModels)

set(CMAKE_CXX_STANDARD 11)

add_executable(ControlModels 
    src/main.cpp 
    src/InputSignal.cpp
    src/Model1_6.cpp
    src/Model2_10.cpp
    src/Model3_4.cpp
)