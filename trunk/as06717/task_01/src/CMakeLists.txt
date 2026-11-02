add_executable(LabWork1
    main.cpp
    input_signal.cpp
    model1-8.cpp
    model2-10.cpp
    model3-2.cpp
)

target_include_directories(LabWork1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(LabWork1 PRIVATE cxx_std_17)