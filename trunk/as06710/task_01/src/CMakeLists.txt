cmake_minimum_required(VERSION 3.15)
project(TMAU_Task_01_Variant_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(tmau_task_01
    main.cpp
    Model_1_1.cpp
    Model_2_3.cpp
    Model_3_5.cpp
    Signal.cpp
)

# plot.py кладём рядом с exe, чтобы system("python plot.py") его нашёл
add_custom_command(TARGET tmau_task_01 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/plot.py
            $<TARGET_FILE_DIR:tmau_task_01>
)