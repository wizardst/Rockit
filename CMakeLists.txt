cmake_minimum_required( VERSION 2.8.8 )

set(RT_BASE_STATIC rt_base_static)
set(RT_MEDIA_STATIC rt_media_static)
set(RT_NODE_STATIC rt_node_static)
set(RT_TASK_STATIC rt_task_static)
set(RT_PLAYER_STATIC rt_player_static)

if (OS_WINDOWS)
    add_definitions(-DOS_WINDOWS)
endif (OS_WINDOWS)

if (OS_LINUX)
    add_definitions(-DOS_LINUX)
    add_definitions(-DRK_HW_CODEC)
endif (OS_LINUX)

if (HAS_PTHREAD)
    add_definitions(-DHAS_PTHREAD)
endif (HAS_PTHREAD)

include_directories(include)

include_directories(third-party/prebuilt/headers/ffmpeg-4.0/include)
include_directories(third-party/prebuilt/headers/alsa-lib/include)
link_directories(third-party/prebuilt/arm32)

include_directories(third-party/drm)

# linked mpp
include_directories(third-party/prebuilt/headers/mpp)

# linked sink-osal (AWindow/AudioTrack/GLES)
include_directories(third-party/prebuilt/headers/sink-osal)

add_subdirectory(src)

message(STATUS "cmake version ${CMAKE_VERSION}")
