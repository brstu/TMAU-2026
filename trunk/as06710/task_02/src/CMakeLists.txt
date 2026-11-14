set(LAB1 ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)

add_executable(tmau_task_02
    main.cpp
    StabilityAnalyzer.cpp
    ${LAB1}/Model_1_1.cpp
    ${LAB1}/Model_2_3.cpp
    ${LAB1}/Model_3_5.cpp
    ${LAB1}/Signal.cpp
)
target_include_directories(tmau_task_02 PRIVATE ${LAB1})
