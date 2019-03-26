cmake_minimum_required( VERSION 2.8.8 )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(RT_BUFFER_SRC
    rt_buffer/rt_buffer.cpp
)

if (HAVE_NDK)
    add_definitions(-DHAVE_NDK)
    message(STATUS "build with HAVE_NDK")
endif()

add_definitions(-DOS_LINUX)
if (OS_ANDROID)
    add_definitions(-DOS_ANDROID)
    set(ANDROID_LIB -llog)
    message(STATUS "build with ANDROID support")
endif()

if (HAVE_DRM)
    add_definitions(-DHAVE_DRM)
    set(DRM_FILES linux/rt_drm.cpp)
    message(STATUS "build with DRM support")
else()
    message(STATUS "build without DRM support")
endif()

set(RT_BASE_LINUX_SRC
    linux/rt_os_cpu_info.cpp
    linux/rt_os_log.cpp
    linux/rt_os_mem.cpp
    linux/rt_os_mutex.cpp
    linux/rt_os_thread.cpp
    linux/rt_os_time.cpp
    linux/rt_os_trust.cpp
    ${DRM_FILES}
    linux/rt_ion.cpp
)

set(RT_BASE_SRC
    rt_array_list.cpp
    rt_dequeue.cpp
    rt_hash_table.cpp
    rt_linked_list.cpp
    rt_mem.cpp
    rt_log.cpp
    rt_string_utils.cpp
    rt_test.cpp
    rt_metadata.cpp
    RTMemService.cpp
    ${RT_BASE_LINUX_SRC}
    ${RT_BUFFER_SRC}
)

add_library(${RT_BASE_STATIC} STATIC ${RT_BASE_SRC})
set_target_properties(${RT_BASE_STATIC} PROPERTIES FOLDER "rt_base")

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_base")
