cmake_minimum_required(VERSION 3.10)
project(Task01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task01
    src/main.cpp
)

target_include_directories(task01 PRIVATE src)

if (MSVC)
    target_compile_options(task01 PRIVATE /W4)
else()
    target_compile_options(task01 PRIVATE -Wall -Wextra -O2)
endif()
