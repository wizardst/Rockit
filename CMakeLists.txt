cmake_minimum_required( VERSION 2.8.8 )

set(RT_BASE_STATIC rt_base_static)
set(RT_MEDIA_STATIC rt_media_static)
set(RT_NODE_STATIC rt_node_static)
set(RT_TASK_STATIC rt_task_static) 

if (OS_WINDOWS)
    add_definitions(-DOS_WINDOWS)
endif (OS_WINDOWS)

if (OS_LINUX)
    add_definitions(-DOS_LINUX)
endif (OS_LINUX)

if (HAS_PTHREAD)
    add_definitions(-DHAS_PTHREAD)
endif (HAS_PTHREAD)

include_directories(include)

include_directories(third-party/prebuilt/headers/ffmpeg-4.0/include)
link_directories(third-party/prebuilt/arm32)

include_directories(third-party/drm)

add_subdirectory(src)

message(STATUS "cmake version ${CMAKE_VERSION}")
