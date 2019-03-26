cmake_minimum_required( VERSION 2.8.8 )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(RT_TASK_TEST_SRC
    rt_task_main.cpp
    test_task_taskpool.cpp
    test_msg_queue.cpp
)

if (OS_ANDROID)
    set(ANDROID_LIBS -llog)
endif()

add_executable(rt_task_test ${RT_TASK_TEST_SRC})
target_link_libraries(rt_task_test ${RT_BASE_STATIC} ${RT_TASK_STATIC} ${ANDROID_LIBS})

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config testcases for rt_task, [Done]")
