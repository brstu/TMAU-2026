include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# На Windows/MSVC библиотека должна использовать тот же CRT, что и тесты
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(LAB1 ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)
set(LAB2 ${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_executable(as06714_unit_tests
    test_lab1_models.cpp
    test_pole_analysis.cpp
    test_excitation_mock.cpp
    ${LAB2}/PoleAnalysis.cpp
    ${LAB2}/Excitation.cpp
    ${LAB2}/Runner.cpp
    ${LAB1}/Model1_7.cpp
    ${LAB1}/Model2_9.cpp
    ${LAB1}/Model3_1.cpp
    ${LAB1}/InputSignal.cpp
)
target_include_directories(as06714_unit_tests PRIVATE ${LAB2} ${LAB1})
target_link_libraries(as06714_unit_tests PRIVATE GTest::gtest_main GTest::gmock)

include(GoogleTest)
gtest_discover_tests(as06714_unit_tests DISCOVERY_TIMEOUT 60)
