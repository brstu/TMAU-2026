cmake_minimum_required(VERSION 3.10)

project(ControlModels LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Ищем main.cpp и InputSignal.cpp в текущей папке ИЛИ в подпапке src
file(GLOB PROJECT_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/InputSignal.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/InputSignal.cpp"
)

add_executable(ControlModels_as06719 ${PROJECT_SOURCES})

if(NOT DEFINED ENV{GITHUB_ACTIONS})
    set_target_properties(ControlModels_as06719 PROPERTIES OUTPUT_NAME "ControlModels")
endif()