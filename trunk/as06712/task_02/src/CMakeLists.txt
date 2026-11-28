set(LAB1 "${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src")

add_executable(task02_app
    "${LAB1}/main.cpp"
)

target_include_directories(task02_app PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${LAB1}"
)
