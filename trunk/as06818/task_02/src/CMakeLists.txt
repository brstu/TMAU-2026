# Ядро задачи 2: модели, симулятор, анализатор устойчивости, экспорт CSV.
# Файл используется и из корневого проекта OTIS (по glob trunk/as0*/*/src/),
# и из локальной сборки task_02/CMakeLists.txt с тестами.
add_library(task02_as06818_core STATIC
    csv_writer.cpp
    diff_model.cpp
    input_signal.cpp
    linear_model.cpp
    nonlinear_model.cpp
    simulator.cpp
    stability_analyzer.cpp
)

target_include_directories(task02_as06818_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(task02_as06818_core PUBLIC cxx_std_17)

add_executable(task02_as06818_app main.cpp)
target_link_libraries(task02_as06818_app PRIVATE task02_as06818_core)
