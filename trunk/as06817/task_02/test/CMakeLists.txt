add_executable(as06817_task02_tests
    test_csv_writer.cpp
    test_input_signal.cpp
    test_model17.cpp
    test_model21.cpp
    test_model35.cpp
    test_simulator.cpp
    test_stability_analyzer.cpp
)

target_include_directories(as06817_task02_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(as06817_task02_tests PRIVATE as06817_task02_core GTest::gmock_main)

include(GoogleTest)
gtest_discover_tests(as06817_task02_tests DISCOVERY_TIMEOUT 30)
