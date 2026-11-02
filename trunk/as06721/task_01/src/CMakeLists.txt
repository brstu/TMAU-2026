cmake_minimum_required(VERSION 3.16)
project(TimayVariant14 LANGUAGES CXX)

add_executable(TimayVariant14 main.cpp)
target_compile_features(TimayVariant14 PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(TimayVariant14 PRIVATE /W4 /utf-8)
else()
    target_compile_options(TimayVariant14 PRIVATE -Wall -Wextra -Wpedantic)
endif()
