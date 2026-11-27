add_executable(var17_unit_tests test_models.cpp test_signals.cpp test_stability.cpp)
target_link_libraries(var17_unit_tests PRIVATE as06802_var17_lib GTest::gtest_main)
if(MSVC)
    target_compile_options(var17_unit_tests PRIVATE /utf-8)
endif()

include(GoogleTest)
gtest_discover_tests(var17_unit_tests DISCOVERY_MODE PRE_TEST NO_PRETTY_VALUES)
