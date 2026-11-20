add_executable(task03_as06818_tests
    test_control_system.cpp
    test_csv_writer.cpp
    test_models.cpp
    test_pid.cpp
)

target_include_directories(task03_as06818_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(task03_as06818_tests
    PRIVATE
        task03_as06818_core
        GTest::gmock_main
)

include(GoogleTest)
gtest_discover_tests(task03_as06818_tests)
