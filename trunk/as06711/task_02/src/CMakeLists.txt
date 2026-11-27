include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(as06711_task02_tests
    test_models.cpp
    test_stability.cpp
)

target_link_libraries(as06711_task02_tests PRIVATE
    GTest::gtest_main
    GTest::gmock
)

target_include_directories(as06711_task02_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

target_compile_features(as06711_task02_tests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(as06711_task02_tests)