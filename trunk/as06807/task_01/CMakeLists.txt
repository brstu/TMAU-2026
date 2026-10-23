cmake_minimum_required(VERSION 3.16)

project(SimulationProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(SimulationProject
    src/main.cpp
    src/simulation.cpp
    src/impact.cpp
    src/LinearModel.cpp
    src/SquareRootModel.cpp
    src/PureLinearDecayModel.cpp
    src/ConsoleUI.cpp
)

target_include_directories(SimulationProject PRIVATE include)