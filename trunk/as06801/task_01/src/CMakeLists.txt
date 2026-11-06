cmake_minimum_required(VERSION 3.10)
project(lab1_models LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исполняемый файл всегда попадает в <build>/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

add_executable(sim main.cpp model.hpp linear_models.hpp nonlinear_models.hpp ode_models.hpp simulator.hpp)
if(MSVC)
  target_compile_options(sim PRIVATE /W3 /utf-8)
  target_compile_definitions(sim PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
  target_compile_options(sim PRIVATE -Wall -Wextra)
endif()