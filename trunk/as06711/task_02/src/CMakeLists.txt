# Локальная копия GoogleTest (чтобы сборка работала без интернета)
add_subdirectory("${CMAKE_SOURCE_DIR}/third_party/googletest"
                 "${CMAKE_BINARY_DIR}/googletest-build"
                 EXCLUDE_FROM_ALL)

add_executable(task02_tests
    test_models.cpp
    test_stability.cpp
)

target_include_directories(task02_tests PRIVATE "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(task02_tests PRIVATE
    gtest
    gmock
    gmock_main
)

include(GoogleTest)
gtest_discover_tests(task02_tests)
