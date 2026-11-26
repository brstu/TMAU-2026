cmake_minimum_required(VERSION 3.16)
project(Variant2Simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(simulation
    main.cpp
    Models.cpp
)

if(MSVC)
    # Явно указываем UTF-8 для русских строковых литералов в исходниках.
    target_compile_options(simulation PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(simulation PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Одинаковый каталог exe для одноконфигурационных и Visual Studio генераторов.
set(OUTPUT_DIR "${CMAKE_BINARY_DIR}/bin")
file(MAKE_DIRECTORY "${OUTPUT_DIR}")
set_target_properties(simulation PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${OUTPUT_DIR}"
)

# Копируем Python-скрипт рядом с исполняемым файлом.
configure_file(plot.py "${OUTPUT_DIR}/plot.py" COPYONLY)
