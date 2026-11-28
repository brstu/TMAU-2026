# Исходники моделей и сигналов — из каталога первой лабораторной работы
set(AS06714_LAB1_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)

add_executable(as06714_task_02
    main.cpp
    PoleAnalysis.cpp
    Excitation.cpp
    Runner.cpp
    ${AS06714_LAB1_DIR}/Model1_7.cpp
    ${AS06714_LAB1_DIR}/Model2_9.cpp
    ${AS06714_LAB1_DIR}/Model3_1.cpp
    ${AS06714_LAB1_DIR}/InputSignal.cpp
)

target_include_directories(as06714_task_02 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${AS06714_LAB1_DIR}
)

# Каталог может подключаться из корневого CMakeLists.txt репозитория,
# поэтому стандарт задаётся свойствами самой цели
set_target_properties(as06714_task_02 PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)
