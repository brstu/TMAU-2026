add_library(task02_core STATIC
    CsvWriter.cpp
    InputSignal.cpp
    Model17.cpp
    Model21.cpp
    Model35.cpp
    Simulator.cpp
    StabilityAnalyzer.cpp
)

target_include_directories(task02_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(task02_core PUBLIC cxx_std_17)

add_executable(task02_app main.cpp)
target_link_libraries(task02_app PRIVATE task02_core)
