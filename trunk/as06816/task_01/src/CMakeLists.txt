cmake_minimum_required(VERSION 3.10)

project(task_01 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task_01
    main.cpp
    Model.h
    DiscreteIntegrator.h
    LogisticModel.h
    InputDriveModel.h
)

# Исходники в UTF-8, чтобы русский текст правильно выводился в консоли
if(MSVC)
    target_compile_options(task_01 PRIVATE /utf-8)
endif()
