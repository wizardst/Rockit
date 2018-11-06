cmake_minimum_required( VERSION 2.8.8 )

set(RT_TASK_SRC
    rt_message.cpp
    rt_msg_handler.cpp
    rt_msg_looper.cpp
    rt_task.cpp
    rt_taskpool.cpp
)

add_library(${RT_TASK_STATIC} STATIC ${RT_TASK_SRC})
set_target_properties(${RT_TASK_STATIC} PROPERTIES FOLDER "rt_task")
target_link_libraries(${RT_TASK_STATIC} ${RT_BASE_STATIC})

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_task")
