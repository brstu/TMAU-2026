cmake_minimum_required(VERSION 3.14)
project(Task02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT MSVC)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(task02_app src/main.cpp)
target_include_directories(task02_app PRIVATE src)

add_executable(task02_tests test/test_stability.cpp test/test_models.cpp)
target_include_directories(task02_tests PRIVATE src)
target_link_libraries(task02_tests PRIVATE GTest::gtest_main GTest::gmock_main)

include(GoogleTest)
gtest_discover_tests(task02_tests)