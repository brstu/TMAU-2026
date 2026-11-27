# --- Лабораторная работа №2, вариант 19 -------------------------------------
add_executable(as06711_task02_app
    main.cpp
)
set_target_properties(as06711_task02_app PROPERTIES
    OUTPUT_NAME as06711_task02
)
target_compile_features(as06711_task02_app PRIVATE cxx_std_17)
target_include_directories(as06711_task02_app PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# --- Тесты (указываем путь в папку test/) -----------------------------------
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(as06711_task02_tests
    ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_models.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../test/test_stability.cpp
)
target_link_libraries(as06711_task02_tests PRIVATE
    GTest::gtest_main
    GTest::gmock
)
target_include_directories(as06711_task02_tests PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_features(as06711_task02_tests PRIVATE cxx_std_17)

include(GoogleTest)
gtest_discover_tests(as06711_task02_tests)