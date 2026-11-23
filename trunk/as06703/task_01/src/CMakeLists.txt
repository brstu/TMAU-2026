cmake_minimum_required(VERSION 3.16)
project(Variant4Modeling LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(variant4 main.cpp)

# Путь к Python-скрипту задаётся относительно расположения CMakeLists.txt.
target_compile_definitions(variant4 PRIVATE
    PLOT_SCRIPT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/plot.py"
)

if (MSVC)
    target_compile_options(variant4 PRIVATE /W4 /utf-8)
else()
    target_compile_options(variant4 PRIVATE -Wall -Wextra -Wpedantic)
endif()
