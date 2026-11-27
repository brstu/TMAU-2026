cmake_minimum_required(VERSION 3.21)
project(As06802Lab02Variant17 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
option(ENABLE_GCC_COVERAGE "Instrument library targets with GCC coverage flags" OFF)

add_subdirectory(src)

if(BUILD_TESTING)
    find_package(GTest CONFIG QUIET)
    if(NOT TARGET GTest::gtest_main)
        include(FetchContent)
        if(POLICY CMP0135)
            cmake_policy(SET CMP0135 NEW)
        endif()
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
        set(local_gtest "${CMAKE_CURRENT_SOURCE_DIR}/build/dependencies/googletest-v1.16.0.tar.gz")
        if(NOT EXISTS "${local_gtest}")
            set(local_gtest "https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.16.0")
        endif()
        FetchContent_Declare(
            googletest
            URL "${local_gtest}"
            URL_HASH SHA256=78c676fc63881529bf97bf9d45948d905a66833fbfa5318ea2cd7478cb98f399
        )
        FetchContent_MakeAvailable(googletest)
        set(nested_gtest "${googletest_SOURCE_DIR}/googletest-1.16.0")
        if(NOT TARGET GTest::gtest_main AND EXISTS "${nested_gtest}/CMakeLists.txt")
            add_subdirectory("${nested_gtest}" "${googletest_BINARY_DIR}")
        endif()
    endif()

    if(NOT TARGET GTest::gtest_main)
        message(FATAL_ERROR "GoogleTest library target was not resolved.")
    endif()
    add_subdirectory(test)
endif()
