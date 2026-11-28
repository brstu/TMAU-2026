cmake_minimum_required(VERSION 3.20)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(TiMAY_Task02_Variant13 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
include(FetchContent)

if(MSVC)
    add_compile_options(/W4 /utf-8)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(simulation_core
    Model.cpp
    InputSignal.cpp
    LinearModel13.cpp
    RelayModel27.cpp
    DifferentialModel31.cpp
    StabilityAnalyzer.cpp
    Simulator.cpp
    ResultWriter.cpp
)
target_include_directories(simulation_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(controlled_object_task02 main.cpp)
target_link_libraries(controlled_object_task02 PRIVATE simulation_core)
target_compile_definitions(controlled_object_task02 PRIVATE PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/..")
set_property(TARGET controlled_object_task02 PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/..")

option(BUILD_TESTING "Build Google Test unit tests" ON)
if(BUILD_TESTING)
    set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT on MSVC" FORCE)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(unit_tests_task02
    ../test/LinearModelTests.cpp
    ../test/RelayModelTests.cpp
    ../test/DifferentialModelTests.cpp
    ../test/StabilityAnalyzerTests.cpp
    ../test/SimulatorTests.cpp
)
target_link_libraries(unit_tests_task02 PRIVATE simulation_core GTest::gtest_main GTest::gmock)
include(GoogleTest)
gtest_discover_tests(unit_tests_task02)
set_target_properties(unit_tests_task02 PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_BINARY_DIR}/bin"
    )
endif()