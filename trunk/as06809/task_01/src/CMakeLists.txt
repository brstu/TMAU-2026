cmake_minimum_required(VERSION 3.10)
project(ControlModels_16)

set(CMAKE_CXX_STANDARD 11)

add_executable(ControlModels_as06809 
    main.cpp 
    InputSignal.cpp
    Model1_6.cpp
    Model2_10.cpp
    Model3_4.cpp
)