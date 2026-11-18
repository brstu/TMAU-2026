include(FetchContent)
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/../src)
set(LAB1 ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)

add_executable(task2_as06720_tests
    test_models.cpp
    test_stability.cpp
    test_signals.cpp
    ${SRC}/StabilityAnalyzer.cpp
    ${SRC}/Simulator.cpp
    ${LAB1}/Model1_5.cpp
    ${LAB1}/Model2_9.cpp
    ${LAB1}/Model3_3.cpp
    ${LAB1}/InputSignal.cpp
)
target_include_directories(task2_as06720_tests PRIVATE
    ${SRC}
    ${LAB1}
)
# gmock_main содержит и Google Test, и Google Mock
target_link_libraries(task2_as06720_tests PRIVATE GTest::gmock_main)

include(GoogleTest)
gtest_discover_tests(task2_as06720_tests)
