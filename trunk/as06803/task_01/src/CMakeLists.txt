cmake_minimum_required(VERSION 3.16)
project(Variant8Simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(model_simulator
    main.cpp
    Model.cpp
)

if(MSVC)
    target_compile_options(model_simulator PRIVATE /W4 /utf-8)
else()
    target_compile_options(model_simulator PRIVATE -Wall -Wextra -Wpedantic)
endif()

file(TO_CMAKE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/plot.py" PLOT_SCRIPT_SOURCE_PATH)

target_compile_definitions(model_simulator PRIVATE
    PLOT_SCRIPT_SOURCE_PATH="${PLOT_SCRIPT_SOURCE_PATH}"
)

add_custom_command(TARGET model_simulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/plot.py"
            "$<TARGET_FILE_DIR:model_simulator>/plot.py"
    VERBATIM
)
