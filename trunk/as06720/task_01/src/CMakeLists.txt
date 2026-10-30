cmake_minimum_required(VERSION 3.10)

project(ControlModels_as06720 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Позволяет собрать проект и напрямую из папки src
file(GLOB PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_executable(ControlModels_as06720 ${PROJECT_SOURCES})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()
