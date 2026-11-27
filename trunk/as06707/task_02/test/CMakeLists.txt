add_executable(run_tests
    test_models.cpp
    test_signals.cpp
    test_stability.cpp
)

target_link_libraries(run_tests PRIVATE core_lib GTest::gtest_main GTest::gmock_main)

include(GoogleTest)
gtest_discover_tests(run_tests)