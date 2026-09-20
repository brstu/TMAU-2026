cmake_minimum_required(VERSION 3.24)
project(tmau_lr1)

set(CMAKE_CXX_STANDARD 17)


set(SOURCE_FILES
    main.cpp
    model_realizations.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

