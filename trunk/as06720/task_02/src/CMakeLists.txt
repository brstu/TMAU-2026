# Модели и сигналы не копируются: берутся из исходников лабораторной работы №1
set(LAB1 ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)

add_executable(tmau_task_02
    main.cpp
    StabilityAnalyzer.cpp
    Simulator.cpp
    ${LAB1}/Model1_5.cpp
    ${LAB1}/Model2_9.cpp
    ${LAB1}/Model3_3.cpp
    ${LAB1}/InputSignal.cpp
)
target_include_directories(tmau_task_02 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${LAB1})
