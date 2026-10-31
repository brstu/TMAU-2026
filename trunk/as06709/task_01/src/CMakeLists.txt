cmake_minimum_required(VERSION 3.16)
project(task_01_variant17 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task_01
    main.cpp
    Model.cpp
    LinearModel17.cpp
    NonlinearModel21.cpp
    DifferentialModel35.cpp
)

if(MSVC)
    # Treat source files and string literals as UTF-8 (including Russian text).
    target_compile_options(task_01 PRIVATE /W4 /utf-8)
else()
    target_compile_options(task_01 PRIVATE -Wall -Wextra -pedantic)
endif()
