cmake_minimum_required(VERSION 3.16)
project(task_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT DEFINED LAB1_SRC OR NOT EXISTS "${LAB1_SRC}/Models.hpp")
    set(_candidates
        "${CMAKE_CURRENT_SOURCE_DIR}/../task_01/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/../task_01/scr"
        "${CMAKE_CURRENT_SOURCE_DIR}/../task_01"
        "${CMAKE_CURRENT_SOURCE_DIR}/../task_01/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/task_01/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/task_01/scr"
        "${CMAKE_CURRENT_SOURCE_DIR}/task_01"
    )
    foreach(cand ${_candidates})
        if (EXISTS "${cand}/Models.hpp")
            get_filename_component(LAB1_SRC "${cand}" ABSOLUTE)
            break()
        endif()
    endforeach()
endif()

if (NOT LAB1_SRC OR NOT EXISTS "${LAB1_SRC}/Models.hpp")
    file(GLOB _siblings LIST_DIRECTORIES true
        RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/.."
        "${CMAKE_CURRENT_SOURCE_DIR}/../*")
    message(FATAL_ERROR
        "\n========================================================\n"
        "Cannot find Models.hpp from Lab 1!\n"
        "Task_02 is at:   ${CMAKE_CURRENT_SOURCE_DIR}\n"
        "Sibling entries: ${_siblings}\n\n"
        "Make sure task_01 is next to task_02 and contains Models.hpp.\n"
        "If task_01 is located elsewhere, configure manually:\n"
        "  cmake .. -DLAB1_SRC=D:/full/path/to/task_01\n"
        "========================================================\n")
endif()

set(LAB1_SRC "${LAB1_SRC}" CACHE PATH "Folder that contains Lab 1 headers (Models.hpp)" FORCE)
message(STATUS "Found Lab 1 sources at: ${LAB1_SRC}")

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_library(task02_core STATIC 
    src/StabilityAnalyzer.cpp
)

target_include_directories(task02_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${LAB1_SRC}
)

if (MSVC)
    target_compile_options(task02_core PRIVATE /utf-8 /W4)
endif()

add_executable(task_02 src/main.cpp)
target_link_libraries(task_02 PRIVATE task02_core)

if (MSVC)
    target_compile_options(task_02 PRIVATE /utf-8 /W4)
endif()

if (EXISTS "${LAB1_SRC}/render_plot.py")
    add_custom_command(TARGET task_02 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${LAB1_SRC}/render_plot.py"
            "$<TARGET_FILE_DIR:task_02>/render_plot.py"
        COMMENT "Copying render_plot.py next to task_02.exe"
    )
endif()

enable_testing()

add_executable(task02_tests
    test/test_models.cpp
    test/test_signals.cpp
    test/test_stability.cpp
    test/test_runner.cpp
    test/test_parametric.cpp
)

target_link_libraries(task02_tests PRIVATE
    task02_core 
    GTest::gtest_main 
    GTest::gmock
)

if (MSVC)
    target_compile_options(task02_core PRIVATE /Zi)
    target_compile_options(task02_tests PRIVATE /Zi)
    set_target_properties(task02_tests PROPERTIES LINK_FLAGS "/DEBUG")
endif()

if (MSVC)
    target_compile_options(task02_tests PRIVATE /utf-8 /W4)
endif()

include(GoogleTest)
gtest_discover_tests(task02_tests
    DISCOVERY_MODE PRE_TEST
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:task02_tests>"
)