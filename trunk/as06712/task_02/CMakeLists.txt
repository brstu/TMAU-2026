# Путь к папке src первой лабы (из папки task_02 выходим на уровень выше)
set(TASK_01_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../task_01/src")

# Основная программа (собирается из исходников task_01)
add_executable(task02_app "${TASK_01_SRC}/main.cpp")
target_include_directories(task02_app PRIVATE src "${TASK_01_SRC}")