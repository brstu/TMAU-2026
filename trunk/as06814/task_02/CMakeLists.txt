cmake_minimum_required(VERSION 3.14)
project(Task02_Variant14 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Включение генерации данных для отчета о покрытии кода (gcov/lcov)
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra --coverage -O0 -g)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Исходный код приложения
add_library(task02_lib
    src/models.h
    src/models.cpp
    src/stability_analyzer.h
    src/stability_analyzer.cpp
)

add_executable(task_02 src/main.cpp)
target_link_libraries(task_02 PRIVATE task02_lib)

# Подключение Google Test и Google Mock через FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# Скрытие вывода сборки gtest
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Сборка тестов
add_executable(unit_tests
    test/test_main.cpp
    test/test_stability.cpp
    test/test_models_param.cpp
    test/test_mock_signal.cpp
)

target_link_libraries(unit_tests PRIVATE
    task02_lib
    gtest
    gmock
)

include(GoogleTest)
gtest_discover_tests(unit_tests)