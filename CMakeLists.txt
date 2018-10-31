cmake_minimum_required( VERSION 2.8.8 )

set(RT_BASE_STATIC rt_base_static)

if (OS_WINDOWS)
    add_definitions(-DOS_WINDOWS)
endif (OS_WINDOWS)

if (OS_LINUX)
    add_definitions(-DOS_LINUX)
endif (OS_LINUX)

include_directories(include)
add_subdirectory(src)

message(STATUS "cmake version ${CMAKE_VERSION}")
