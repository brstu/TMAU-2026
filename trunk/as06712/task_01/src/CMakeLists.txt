cmake_minimum_required(VERSION 3.10)

add_executable(task01
    main.cpp
)

target_include_directories(task01 PRIVATE .)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
