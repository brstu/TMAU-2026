# Модели и сигналы берутся из лабораторной работы №1 (без копирования)
set(LAB1 ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)

add_executable(as06720_task_02
    main.cpp
    StabilityAnalyzer.cpp
    Simulator.cpp
    ${LAB1}/Model1_5.cpp
    ${LAB1}/Model2_9.cpp
    ${LAB1}/Model3_3.cpp
    ${LAB1}/InputSignal.cpp
)
target_include_directories(as06720_task_02 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${LAB1})
# Каталог src подключается и из корневого CMakeLists.txt репозитория,
# поэтому стандарт задаётся для цели, а не для проекта
set_target_properties(as06720_task_02 PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
