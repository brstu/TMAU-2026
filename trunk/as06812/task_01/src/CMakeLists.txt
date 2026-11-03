cmake_minimum_required(VERSION 3.10)
project(as06812_lab1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "*.cpp")

add_executable(as06812_lab1 ${SOURCES})