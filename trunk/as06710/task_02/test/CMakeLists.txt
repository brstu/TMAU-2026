include(FetchContent)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(LAB1 ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)

add_executable(task2_as06710_tests
    test_models.cpp
    test_stability.cpp
    test_signals.cpp
    ../src/StabilityAnalyzer.cpp
    ${LAB1}/Model_1_1.cpp
    ${LAB1}/Model_2_3.cpp
    ${LAB1}/Model_3_5.cpp
    ${LAB1}/Signal.cpp
)
target_include_directories(task2_as06710_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${LAB1}
)
target_link_libraries(task2_as06710_tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(task2_as06710_tests)
