cmake_minimum_required(VERSION 3.10)

project(ControlModels LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Все исходники проекта лежат в подпапке src
file(GLOB PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(ControlModels ${PROJECT_SOURCES})
target_include_directories(ControlModels PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

if(MSVC)
    target_compile_options(ControlModels PRIVATE /W4)
else()
    target_compile_options(ControlModels PRIVATE -Wall -Wextra)
endif()
