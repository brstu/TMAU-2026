add_library(as06817_task02_core STATIC
    CsvWriter.cpp
    InputSignal.cpp
    Model17.cpp
    Model21.cpp
    Model35.cpp
    Simulator.cpp
    StabilityAnalyzer.cpp
)

target_include_directories(as06817_task02_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(as06817_task02_core PUBLIC cxx_std_17)

add_executable(as06817_task02_app main.cpp)
target_link_libraries(as06817_task02_app PRIVATE as06817_task02_core)
