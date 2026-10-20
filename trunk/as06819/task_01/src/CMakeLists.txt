cmake_minimum_required(VERSION 3.10)
project(ControlModels)

set(CMAKE_CXX_STANDARD 11)

# Перечисляем все файлы реализации проекта для сборки
add_executable(ControlModels 
    main.cpp 
    InputSignal.cpp
    Model1_9.cpp
    Model2_3.cpp
    Model3_7.cpp
)
