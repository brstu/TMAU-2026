cmake_minimum_required(VERSION 3.16)

project(task_01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(lab1
    main.cpp
)

target_include_directories(lab1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(lab1 PRIVATE /W4 /permissive-)
else()
    target_compile_options(lab1 PRIVATE -Wall -Wextra -Wpedantic)
endif()