add_executable(task_01
    main.cpp
)

target_include_directories(task_01 PRIVATE .)

if (MSVC)
    target_compile_options(task_01 PRIVATE /W4 /utf-8)
else()
    target_compile_options(task_01 PRIVATE -Wall -Wextra -O2)
endif()