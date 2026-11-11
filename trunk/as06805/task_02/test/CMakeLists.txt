cmake_minimum_required(VERSION 3.15)

# ---------------------------------------------------------------------------
# Google Test / Google Mock
# ---------------------------------------------------------------------------
# Основной вариант — вендорённая копия (third_party/googletest), чтобы сборка
# и запуск тестов работали без доступа в Интернет.
# Резервный вариант — автоматическая загрузка через FetchContent.
set(TMAU_GTEST_VENDORED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../third_party)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK ON CACHE BOOL "" FORCE)

if(EXISTS ${TMAU_GTEST_VENDORED_DIR}/CMakeLists.txt)
    message(STATUS "Google Test: используется вендорённая копия ${TMAU_GTEST_VENDORED_DIR}")
    add_subdirectory(${TMAU_GTEST_VENDORED_DIR} ${CMAKE_BINARY_DIR}/googletest EXCLUDE_FROM_ALL)
else()
    message(STATUS "Google Test: загрузка через FetchContent (v1.17.0)")
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.17.0
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(googletest)
endif()

include(GoogleTest)

# ---------------------------------------------------------------------------
# Модульные тесты
# ---------------------------------------------------------------------------
add_executable(task_02_tests
    test_main.cpp
    ConsoleAppTest.cpp
    LinearModel11Test.cpp
    LinearModel13Test.cpp
    NonlinearModel22Test.cpp
    NonlinearModel23Test.cpp
    NonlinearModel25Test.cpp
    ODEModel37Test.cpp
    SignalGeneratorTest.cpp
    SimulatorTest.cpp
    StabilityAnalyzerTest.cpp
    MockModelTest.cpp
)

target_link_libraries(task_02_tests PRIVATE tmau_models gtest gmock)

if(MSVC)
    target_compile_options(task_02_tests PRIVATE /W4 /utf-8)
else()
    target_compile_options(task_02_tests PRIVATE -Wall -Wextra -O2)
endif()

if(TMAU_ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(task_02_tests PRIVATE --coverage -O0 -g)
    target_link_options(task_02_tests PRIVATE --coverage)
endif()

# Регистрация каждого теста в CTest (запуск по одному + полный прогон).
# Используется gtest_add_tests, т.к. gtest_discover_tests создаёт служебный
# JSON-файл в каталоге сборки, а при кириллице в пути gtest не может его создать.
gtest_add_tests(TARGET task_02_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    TEST_PREFIX "gtest:")
