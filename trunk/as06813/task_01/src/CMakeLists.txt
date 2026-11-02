cmake_minimum_required(VERSION 3.10)
project(TMAU_Lab1 CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(TMAU_Lab1
    main.cpp
    Input.h
    Model.h
    LinearModel.h
    NonlinearModel.h
    DifferentialModel.h
)

# русские строки в исходниках (UTF-8) для компилятора Visual Studio
if(MSVC)
    target_compile_options(TMAU_Lab1 PRIVATE /utf-8)
endif()
