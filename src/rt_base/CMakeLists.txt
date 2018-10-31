cmake_minimum_required( VERSION 2.8.8 )

set(RT_BASE_SRC
    rt_array_list.cpp
    rt_dequeue.cpp
    rt_hash_table.cpp
    rt_linked_list.cpp
    rt_mem.cpp
    rt_mem_service.cpp
    rt_log.cpp
    linux/rt_os_mem.cpp
    linux/rt_os_log.cpp
)

add_library(${RT_BASE_STATIC} STATIC ${RT_BASE_SRC})
set_target_properties(${RT_BASE_STATIC} PROPERTIES FOLDER "rt_base")

add_subdirectory(test)

message(STATUS "cmake version ${CMAKE_VERSION}")
