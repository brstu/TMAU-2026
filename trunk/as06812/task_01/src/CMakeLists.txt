cmake_minimum_required(VERSION 3.10)

project(task01_variant10 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(task_01_as06812
    RealizeModel.cpp
    models.h
)

# Оставляем привычное имя исполняемого файла
set_target_properties(task_01_as06812 PROPERTIES
    OUTPUT_NAME task_01
)

if(MSVC)
    target_compile_options(task_01_as06812 PRIVATE /utf-8 /W4)
else()
    target_compile_options(task_01_as06812 PRIVATE -Wall -Wextra)
endif()