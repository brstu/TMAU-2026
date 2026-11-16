add_executable(task02_as06818_tests
    test_csv_writer.cpp
    test_diff_model.cpp
    test_input_signal.cpp
    test_linear_model.cpp
    test_nonlinear_model.cpp
    test_simulator.cpp
    test_stability.cpp
)

target_include_directories(task02_as06818_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(task02_as06818_tests
    PRIVATE
        task02_as06818_core
        GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(task02_as06818_tests)
