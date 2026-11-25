cmake_minimum_required(VERSION 3.21)
project(Task02Variant21 LANGUAGES CXX)

include(CTest)
option(TASK02_COVERAGE "Collect coverage of the simulation and stability analysis using GCC" OFF)

add_subdirectory(src)

if(BUILD_TESTING)
    find_package(GTest CONFIG QUIET)
    if(NOT TARGET GTest::gtest_main)
        include(FetchContent)
        if(POLICY CMP0135)
            cmake_policy(SET CMP0135 NEW)
        endif()
        set(gtest_force_shared_crt ON CACHE BOOL "Match the Visual Studio runtime")
        set(BUILD_GMOCK OFF CACHE BOOL "Use injected input signals in tests")
        set(googletest_archive "${CMAKE_CURRENT_SOURCE_DIR}/build/dependencies/googletest-v1.16.0.tar.gz")
        if(NOT EXISTS "${googletest_archive}")
            set(googletest_archive "https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.16.0")
        endif()
        FetchContent_Declare(googletest
            URL "${googletest_archive}"
            URL_HASH SHA256=78c676fc63881529bf97bf9d45948d905a66833fbfa5318ea2cd7478cb98f399
        )
        FetchContent_MakeAvailable(googletest)
        # CMake can leave the archive's top-level directory in place when
        # the extraction path contains square brackets.
        set(googletest_nested_dir "${googletest_SOURCE_DIR}/googletest-1.16.0")
        if(NOT TARGET GTest::gtest_main AND EXISTS "${googletest_nested_dir}/CMakeLists.txt")
            add_subdirectory("${googletest_nested_dir}" "${googletest_BINARY_DIR}")
        endif()
    endif()

    if(NOT TARGET GTest::gtest_main)
        message(FATAL_ERROR "Google Test was not configured. Check the Google Test source directory.")
    endif()
    add_subdirectory(test)
endif()
