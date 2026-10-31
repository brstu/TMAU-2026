cmake_minimum_required(VERSION 3.16)

project(SimulationProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(SimulationProject
    main.cpp
    simulation.cpp
    impact.cpp
    LinearModel.cpp
    SquareRootModel.cpp
    PureLinearDecayModel.cpp
    ConsoleUI.cpp
)

target_include_directories(SimulationProject PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include")
