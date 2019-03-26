cmake_minimum_required( VERSION 2.8.8 )

set(RT_BASE_STATIC rt_base_static)
set(RT_MEDIA_STATIC rt_media_static)
set(RT_NODE_STATIC rt_node_static)
set(RT_TASK_STATIC rt_task_static)
set(RT_PLAYER_STATIC rt_player_static)
set(RT_PLAYER_SHARED rt_player_shared)

if (OS_WINDOWS)
    add_definitions(-DOS_WINDOWS)
    add_definitions(-DHAS_PTHREAD)
endif (OS_WINDOWS)

if (OS_LINUX)
    add_definitions(-DOS_LINUX)
    add_definitions(-DRK_HW_CODEC)
    add_definitions(-DHAS_PTHREAD)
endif (OS_LINUX)

if (OS_ANDROID)
    add_definitions(-DOS_ANDROID)
endif (OS_ANDROID)

include_directories(include)

# include third-party pre-built headers
if (FFMPEG_SINGLE)
    include_directories(third-party/prebuilt/headers/ffmpeg-4.0/include)
endif()

include_directories(third-party/prebuilt/headers/alsa-lib/include)
include_directories(third-party/drm)
include_directories(third-party/prebuilt/headers/mpp)
include_directories(third-party/prebuilt/headers/sink-osal)

# link third-party pre-built libraries
if( X86_64 OR ARM64_V8A )
    link_directories(third-party/prebuilt/arm64)
    link_directories(${LINUX_SYSROOT}/usr/lib)
    message(STATUS "build with ARM64 prebilt libraries")
else()
    link_directories(third-party/prebuilt/arm32)
    # link_directories(${LINUX_SYSROOT}/usr/lib)
    message(STATUS "build with ARM32 prebilt libraries")
endif()

add_subdirectory(src)

message(STATUS "cmake version ${CMAKE_VERSION}")
