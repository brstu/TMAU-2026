cmake_minimum_required(VERSION 3.14)
project(tmau_lr2 LANGUAGES CXX)

message(STATUS "Compiler ID: ${CMAKE_CXX_COMPILER_ID}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MODELS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../task_01/src")

if(NOT EXISTS "${MODELS_DIR}/model_realizations.cpp")
    message(FATAL_ERROR "Не найден внешний файл моделей: ${MODELS_DIR}")
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

include(FetchContent)
FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_library(ModelsLib STATIC "${MODELS_DIR}/model_realizations.cpp")
target_include_directories(ModelsLib PUBLIC "${MODELS_DIR}")

add_library(StabilityLib STATIC src/stability_analyzer.cpp)
target_include_directories(StabilityLib PUBLIC include)

# add_executable(MainApp src/main.cpp)
# target_link_libraries(MainApp PUBLIC ModelsLib StabilityLib)


add_executable(UnitTests test/model_tests.cpp)
target_link_options(UnitTests PRIVATE $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:GNU,Clang>>:--coverage>)
target_link_libraries(UnitTests PRIVATE ModelsLib StabilityLib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(UnitTests)