cmake_minimum_required(VERSION 3.16)
project(ControlledObjectVariant13 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(controlled_object src/main.cpp)

if(MSVC)
    target_compile_options(controlled_object PRIVATE /W4 /utf-8)
else()
    target_compile_options(controlled_object PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Keep generated CSV in the project root, regardless of the working directory.
target_compile_definitions(controlled_object PRIVATE PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}")

# The C++ program changes to the project root before launching plot.py via py on Windows.
set_property(TARGET controlled_object PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
