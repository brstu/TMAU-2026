cmake_minimum_required(VERSION 3.15)

# Корневой файл проекта лабораторной работы №2 (задание 2):
# модульные тесты (Google Test) + модуль анализа устойчивости + локальная автоматизация.
project(TMAU_Task02_StabilityAndTests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Сборка модульных тестов (Google Test / Google Mock).
option(TMAU_BUILD_TESTS "Собрать модульные тесты (Google Test)" ON)

# Инструментирование кода для последующего анализа покрытия (gcovr).
option(TMAU_ENABLE_COVERAGE "Включить инструментирование кода для анализа покрытия" OFF)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_subdirectory(src)

if(TMAU_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
