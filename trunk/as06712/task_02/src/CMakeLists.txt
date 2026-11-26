# Указываем путь к папке src первой лабы (на уровень выше и в соседнюю папку)
set(TASK_01_SRC "${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src")

# Берем main.cpp из первой лабы
add_executable(task02_app "${TASK_01_SRC}/main.cpp")

# Ищем заголовочные файлы в текущей папке (task_02/src) и в первой лабе
target_include_directories(task02_app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" "${TASK_01_SRC}")