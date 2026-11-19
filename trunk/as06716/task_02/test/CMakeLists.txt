include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    GIT_SHALLOW TRUE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(task02_tests test_models.cpp test_stability.cpp test_simulation.cpp test_app.cpp)
target_link_libraries(task02_tests PRIVATE task02_core GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(task02_tests DISCOVERY_MODE PRE_TEST)
