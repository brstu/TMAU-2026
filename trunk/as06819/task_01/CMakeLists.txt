cmake_minimum_required(VERSION 3.10)
project(ControlModels)

set(CMAKE_CXX_STANDARD 11)

# Перечисляем все файлы реализации проекта для сборки
add_executable(ControlModels 
    src/main.cpp 
    src/InputSignal.cpp
    src/Model1_9.cpp
    src/Model2_3.cpp
    src/Model3_7.cpp
)
