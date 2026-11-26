add_executable(var16_test_runner test_models.cpp test_signals.cpp test_stability.cpp)
target_link_libraries(var16_test_runner PRIVATE as06809_var16_lib GTest::gtest_main)
if(MSVC)
    target_compile_options(var16_test_runner PRIVATE /utf-8)
endif()

include(GoogleTest)
gtest_discover_tests(var16_test_runner DISCOVERY_MODE PRE_TEST NO_PRETTY_VALUES)
