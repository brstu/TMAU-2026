cmake_minimum_required(VERSION 3.14)

project(task_02)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/heads/main.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(task_02_tests
    src/models.cpp
    src/stability.cpp
    src/signals.cpp
    test/test_models.cpp
    test/test_stability.cpp
    test/test_parameterized.cpp
    test/test_signals.cpp
)

target_link_libraries(task_02_tests
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(task_02_tests)

# Демонстрационная программа с проверкой устойчивости
add_executable(task_02_demo
    src/main.cpp
    src/models.cpp
    src/stability.cpp
)