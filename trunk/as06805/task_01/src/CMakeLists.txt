cmake_minimum_required(VERSION 3.10)
project(task_01_as06805)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task_01_as06805
    main.cpp
    LinearModel13.h
    NonlinearModel25.h
    ODEModel37.h
    Model.h
    SignalGenerator.h
    Simulator.h
)

target_include_directories(task_01_as06805 PRIVATE .)

if (MSVC)
    target_compile_options(task_01_as06805 PRIVATE /W4 /utf-8)
else()
    target_compile_options(task_01_as06805 PRIVATE -Wall -Wextra -O2)
endif()