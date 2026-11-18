cmake_minimum_required(VERSION 3.15)
project(TMAU_as06720_task_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(src)

enable_testing()
add_subdirectory(test)
