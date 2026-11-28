cmake_minimum_required(VERSION 3.16)
project(task_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Так как мы теперь внутри src, путь до первой лабы: ../../task_01
if (NOT DEFINED LAB1_SRC OR NOT EXISTS "${LAB1_SRC}/Models.hpp")
    set(_candidates
        "${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/scr"
        "${CMAKE_CURRENT_SOURCE_DIR}/../../task_01"
    )
    foreach(cand ${_candidates})
        if (EXISTS "${cand}/Models.hpp")
            get_filename_component(LAB1_SRC "${cand}" ABSOLUTE)
            break()
        endif()
    endforeach()
endif()

if (NOT LAB1_SRC OR NOT EXISTS "${LAB1_SRC}/Models.hpp")
    message(FATAL_ERROR "Cannot find Models.hpp from Lab 1!")
endif()

add_library(task02_core STATIC StabilityAnalyzer.cpp)
target_include_directories(task02_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LAB1_SRC}
)
if (MSVC)
    target_compile_options(task02_core PRIVATE /utf-8 /W4 /Zi)
endif()

add_executable(task_02 main.cpp)
target_link_libraries(task_02 PRIVATE task02_core)
if (MSVC)
    target_compile_options(task_02 PRIVATE /utf-8 /W4)
endif()

if (EXISTS "${LAB1_SRC}/render_plot.py")
    add_custom_command(TARGET task_02 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${LAB1_SRC}/render_plot.py"
            "$<TARGET_FILE_DIR:task_02>/render_plot.py"
    )
endif()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    message(STATUS "Local build detected: enabling Google Test")
    
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(task02_tests
        ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_models.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_signals.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_stability.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_runner.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_parametric.cpp
    )
    target_link_libraries(task02_tests PRIVATE
        task02_core GTest::gtest_main GTest::gmock
    )
    if (MSVC)
        target_compile_options(task02_tests PRIVATE /utf-8 /W4 /Zi)
        set_target_properties(task02_tests PROPERTIES LINK_FLAGS "/DEBUG")
    endif()

    include(GoogleTest)
    gtest_discover_tests(task02_tests
        DISCOVERY_MODE PRE_TEST
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:task02_tests>"
    )
else()
    message(STATUS "CI build detected: tests are disabled to save time.")
endif()