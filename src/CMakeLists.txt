cmake_minimum_required( VERSION 2.8.8 )

include_directories(rt_base/include)
include_directories(rt_node/include)
include_directories(rt_node/ff_node/include)
include_directories(rt_task/include)

add_subdirectory(rt_base)
add_subdirectory(rt_node)
add_subdirectory(rt_task)
add_subdirectory(tests)

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config source codes")
