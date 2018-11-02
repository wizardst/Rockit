cmake_minimum_required( VERSION 2.8.8 )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(RT_TASK_TEST_SRC
    rt_task_main.cpp
    test_task_taskpool.cpp
)

add_executable(rt_task_test ${RT_TASK_TEST_SRC})
target_link_libraries(rt_task_test ${RT_BASE_STATIC} ${RT_TASK_STATIC})
message(STATUS "cmake version ${CMAKE_VERSION} [@@]config testcases for rt_task")
