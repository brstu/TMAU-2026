add_executable(task02_tests test_models.cpp test_signals.cpp test_stability.cpp)
target_link_libraries(task02_tests PRIVATE as06819_task02_models GTest::gtest_main)
if(MSVC)
    target_compile_options(task02_tests PRIVATE /utf-8)
endif()

include(GoogleTest)
gtest_discover_tests(task02_tests DISCOVERY_MODE PRE_TEST NO_PRETTY_VALUES)
