cmake_minimum_required(VERSION 3.21)
project(as06721_task03 LANGUAGES CXX)

include(CTest)
add_subdirectory(src)

if(BUILD_TESTING)
    find_package(GTest CONFIG QUIET)
    if(NOT TARGET GTest::gtest_main)
        include(FetchContent)
        if(POLICY CMP0135)
            cmake_policy(SET CMP0135 NEW)
        endif()
        set(gtest_force_shared_crt ON CACHE BOOL "Match the parent MSVC runtime")
        set(BUILD_GMOCK OFF CACHE BOOL "This lab uses Google Test")
        set(gtest_archive "${CMAKE_CURRENT_SOURCE_DIR}/build/dependencies/googletest-v1.16.0.tar.gz")
        if(NOT EXISTS "${gtest_archive}")
            set(gtest_archive "https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.16.0")
        endif()
        FetchContent_Declare(googletest
            URL "${gtest_archive}"
            URL_HASH SHA256=78c676fc63881529bf97bf9d45948d905a66833fbfa5318ea2cd7478cb98f399
            TIMEOUT 45
        )
        FetchContent_MakeAvailable(googletest)
        # Some CMake extractors leave the top directory in paths containing brackets.
        if(NOT TARGET GTest::gtest_main AND
           EXISTS "${googletest_SOURCE_DIR}/googletest-1.16.0/CMakeLists.txt")
            add_subdirectory("${googletest_SOURCE_DIR}/googletest-1.16.0" "${googletest_BINARY_DIR}")
        endif()
    endif()
    add_subdirectory(test)
endif()
