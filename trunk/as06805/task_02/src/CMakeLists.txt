cmake_minimum_required(VERSION 3.10)
project(task_02_as06805 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Библиотека с моделями и модулем анализа устойчивости.
add_library(tmau_models STATIC
    ConsoleApp.cpp
    ConsoleApp.h
    Model.h
    LinearModel11.cpp
    LinearModel11.h
    LinearModel13.cpp
    LinearModel13.h
    NonlinearModel22.cpp
    NonlinearModel22.h
    NonlinearModel23.cpp
    NonlinearModel23.h
    NonlinearModel25.cpp
    NonlinearModel25.h
    ODEModel37.cpp
    ODEModel37.h
    SignalGenerator.cpp
    SignalGenerator.h
    Simulator.cpp
    Simulator.h
    StabilityAnalyzer.cpp
    StabilityAnalyzer.h
)

target_include_directories(tmau_models PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Основное приложение (интерактивная симуляция + предупреждение об устойчивости).
add_executable(task_02_as06805 main.cpp)
target_link_libraries(task_02_as06805 PRIVATE tmau_models)

if(MSVC)
    target_compile_options(tmau_models PRIVATE /W4 /utf-8)
    target_compile_options(task_02_as06805 PRIVATE /W4 /utf-8)
else()
    target_compile_options(tmau_models PRIVATE -Wall -Wextra -O2)
    target_compile_options(task_02_as06805 PRIVATE -Wall -Wextra -O2)
endif()

# Инструментирование кода для анализа покрытия (gcovr / lcov).
if(TMAU_ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(tmau_models PUBLIC --coverage -O0 -g)
    target_link_options(tmau_models PUBLIC --coverage)
    target_compile_options(task_02_as06805 PRIVATE --coverage -O0 -g)
    target_link_options(task_02_as06805 PRIVATE --coverage)
endif()
