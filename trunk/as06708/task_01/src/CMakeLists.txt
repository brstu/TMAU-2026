cmake_minimum_required(VERSION 3.5...4.4.0)
project(tmau_lr1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(SOURCE_FILES
    main.cpp
    model_realizations.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(tmau_lr1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})