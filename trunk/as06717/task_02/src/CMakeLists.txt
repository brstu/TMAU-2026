set(LAB1 ${CMAKE_CURRENT_SOURCE_DIR}/../../task_01/src)
add_executable(tmau_task_02
    main.cpp
    stability_core.cpp
    ${LAB1}/model1-8.cpp
    ${LAB1}/model2-10.cpp
    ${LAB1}/model3-2.cpp
    ${LAB1}/input_signal.cpp
)
target_include_directories(tmau_task_02 PRIVATE ${LAB1})