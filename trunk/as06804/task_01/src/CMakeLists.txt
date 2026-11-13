cmake_minimum_required(VERSION 3.10)
project(Lab1_Modeling)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  # <--- ДОБАВИТЬ ЭТУ СТРОКУ

# Ищем все cpp файлы прямо в текущей папке
file(GLOB SOURCES "*.cpp")

add_executable(lab1_app ${SOURCES})
