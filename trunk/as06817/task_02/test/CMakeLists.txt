add_executable(task02_tests
    test_csv_writer.cpp
    test_input_signal.cpp
    test_model17.cpp
    test_model21.cpp
    test_model35.cpp
    test_simulator.cpp
    test_stability_analyzer.cpp
)

target_include_directories(task02_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(task02_tests PRIVATE task02_core GTest::gmock_main)

include(GoogleTest)
gtest_discover_tests(task02_tests)
