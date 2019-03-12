cmake_minimum_required( VERSION 2.8.8 )

set(RT_BUFFER_SRC
    rt_buffer/rt_buffer.cpp
)

add_definitions(-DOS_LINUX)

if (HAVE_DRM)
    add_definitions(-DHAVE_DRM)
    set(DRM_FILES linux/rt_drm.cpp)
    message(STATUS "build with DRM Support")
else()
    message(STATUS "build without DRM Support")
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
