cmake_minimum_required(VERSION 3.14)
project(DiscreteSim CXX)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0")
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5 CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra)
endif()

# Покрытие — только для GCC/Clang
option(ENABLE_COVERAGE "Enable coverage" OFF)
if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

# Основная библиотека (файлы теперь лежат в этой же папке)
add_library(simcore STATIC stability_analyzer.cpp)
target_include_directories(simcore PUBLIC .)

# Приложение (main.cpp в этой же папке)
add_executable(aaaaa main.cpp)
target_link_libraries(aaaaa PRIVATE simcore)

# Тесты
enable_testing()
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Тесты лежат в папке ../test (поднимаемся на уровень выше из src)
add_executable(sim_tests
    ../test/test_models.cpp
    ../test/test_stability.cpp
    ../test/test_inputs.cpp
)
target_link_libraries(sim_tests PRIVATE simcore GTest::gtest_main GTest::gmock)

# Указываем компилятору искать заголовки в src и test
target_include_directories(sim_tests PRIVATE . ../test)

include(GoogleTest)
gtest_discover_tests(sim_tests)