# Ядро задачи 3: объекты управления из ЛР1, ПИД-регулятор, замкнутая
# система с режимами симуляции, экспорт CSV.
# Файл используется и из корневого проекта OTIS (по glob trunk/as0*/*/src/),
# и из локальной сборки task_03/CMakeLists.txt с тестами.
add_library(task03_as06818_core STATIC
    control_system.cpp
    csv_writer.cpp
    diff_model.cpp
    linear_model.cpp
    nonlinear_model.cpp
    pid.cpp
)

target_include_directories(task03_as06818_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(task03_as06818_core PUBLIC cxx_std_17)

add_executable(task03_as06818_app main.cpp)
target_link_libraries(task03_as06818_app PRIVATE task03_as06818_core)
