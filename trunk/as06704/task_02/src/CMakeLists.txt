add_library(timay_core
    model.cpp
    stability_analyzer.cpp
    input_signal.cpp
    simulator.cpp
    csv_writer.cpp
)

target_include_directories(timay_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(timay_core PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(timay_core PRIVATE --coverage -O0 -g)
    target_link_options(timay_core INTERFACE --coverage)
endif()

add_executable(TiMAY main.cpp)
target_link_libraries(TiMAY PRIVATE timay_core)

set_target_properties(TiMAY PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
foreach(CONFIG Debug Release RelWithDebInfo MinSizeRel)
    string(TOUPPER "${CONFIG}" CONFIG_UPPER)
    set_target_properties(TiMAY PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_BINARY_DIR}/bin"
    )
endforeach()
