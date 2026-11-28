get_filename_component(LAB1_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../task_01" ABSOLUTE)

add_library(core_lib STATIC
    StabilityAnalyzer.cpp
    ${LAB1_DIR}/src/Model1_1.cpp
    ${LAB1_DIR}/src/Model2_5.cpp
    ${LAB1_DIR}/src/Model3_9.cpp
)

target_include_directories(core_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LAB1_DIR}/src
)

add_executable(tmau_task_02 main.cpp)
target_link_libraries(tmau_task_02 PRIVATE core_lib)

target_compile_definitions(tmau_task_02 PRIVATE PYTHON_SCRIPT_PATH="${LAB1_DIR}/scripts/plot_results.py")