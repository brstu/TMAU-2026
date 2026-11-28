cmake_minimum_required(VERSION 3.16)
project(task_02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(TASK02_BUILD_TESTS "Build Google Test based unit tests" ON)
option(TASK02_ENABLE_COVERAGE "Instrument code with gcov (GCC/Clang only)" ON)

set(TASK02_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(TASK02_TEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../test")

set(TASK02_CORE_SOURCES
    "${TASK02_SRC_DIR}/LinearModel.cpp"
    "${TASK02_SRC_DIR}/NonLinearModel.cpp"
    "${TASK02_SRC_DIR}/DiffEqModel.cpp"
    "${TASK02_SRC_DIR}/InputSignal.cpp"
    "${TASK02_SRC_DIR}/SimulationRunner.cpp"
    "${TASK02_SRC_DIR}/StabilityAnalyzer.cpp"
)

add_library(task02_core STATIC ${TASK02_CORE_SOURCES})
target_include_directories(task02_core PUBLIC "${TASK02_SRC_DIR}")

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" AND TASK02_ENABLE_COVERAGE)
    target_compile_options(task02_core PUBLIC -O0 -g --coverage)
    target_link_options(task02_core PUBLIC --coverage)
elseif(MSVC)
    target_compile_options(task02_core PRIVATE /utf-8 /W4 /Zi)
endif()

add_executable(task_02 "${TASK02_SRC_DIR}/main.cpp")
target_link_libraries(task_02 PRIVATE task02_core)
if(MSVC)
    target_compile_options(task_02 PRIVATE /utf-8 /W4)
endif()

if(TASK02_BUILD_TESTS)
    include(FetchContent)

    # Optional offline mode: set GTEST_SOURCE_DIR to a local googletest checkout.
    if(DEFINED ENV{GTEST_SOURCE_DIR} AND EXISTS "$ENV{GTEST_SOURCE_DIR}/CMakeLists.txt")
        set(FETCHCONTENT_SOURCE_DIR_GOOGLETEST "$ENV{GTEST_SOURCE_DIR}" CACHE PATH "" FORCE)
    endif()

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
        "${TASK02_TEST_DIR}/test_models.cpp"
        "${TASK02_TEST_DIR}/test_stability.cpp"
        "${TASK02_TEST_DIR}/test_signals.cpp"
        "${TASK02_TEST_DIR}/test_runner.cpp"
        "${TASK02_TEST_DIR}/test_parametric.cpp"
    )
    target_link_libraries(task02_tests PRIVATE
        task02_core
        GTest::gtest_main
        GTest::gmock
    )
    if(MSVC)
        target_compile_options(task02_tests PRIVATE /utf-8 /W4 /Zi)
    endif()

    include(GoogleTest)
    gtest_discover_tests(task02_tests
        DISCOVERY_MODE PRE_TEST
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    )
endif()
