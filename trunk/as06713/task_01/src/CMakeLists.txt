
cmake_minimum_required(VERSION 3.20)

project(task_01_as06713 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(task_01_as06713 main.cpp)

# Keep the executable filename unchanged
set_target_properties(task_01_as06713 PROPERTIES
    OUTPUT_NAME task_01
)

if(MSVC)
    target_compile_options(task_01_as06713 PRIVATE /utf-8)
endif()
