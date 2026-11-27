cmake_minimum_required(VERSION 3.16)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

project(TiMAY_Variant12_Task02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
option(ENABLE_COVERAGE "Enable gcov-compatible coverage for GCC or Clang" OFF)

if(BUILD_TESTING)
    include(FetchContent)
    find_package(GTest QUIET)

    if(NOT GTest_FOUND)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        )
        FetchContent_MakeAvailable(googletest)
    endif()
endif()

add_subdirectory(src)

if(BUILD_TESTING)
    add_executable(TiMAY_Tests
        test/model_tests.cpp
        test/stability_tests.cpp
        test/input_signal_tests.cpp
        test/simulator_tests.cpp
        test/euler_convergence_tests.cpp
        test/csv_writer_tests.cpp
    )

    target_link_libraries(TiMAY_Tests PRIVATE timay_core GTest::gtest_main)

    target_compile_options(TiMAY_Tests PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
        $<$<CXX_COMPILER_ID:MSVC>:/W4>
    )

    if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(TiMAY_Tests PRIVATE --coverage -O0 -g)
        target_link_options(TiMAY_Tests PRIVATE --coverage)
    endif()

    set_target_properties(TiMAY_Tests PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test"
    )
    foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
        string(TOUPPER "${CONFIG}" CONFIG_UPPER)
        set_target_properties(TiMAY_Tests PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}/test"
        )
    endforeach()

    include(GoogleTest)
    gtest_discover_tests(TiMAY_Tests DISCOVERY_MODE PRE_TEST)
endif()
