cmake_minimum_required( VERSION 2.8.8 )

set(RT_BUFFER_SRC
    rt_buffer/rt_buffer.cpp
    rt_buffer/rt_media_frame.cpp
    rt_buffer/rt_media_packet.cpp
)

set(RT_BASE_LINUX_SRC
    linux/rt_os_cpu_info.cpp
    linux/rt_os_log.cpp
    linux/rt_os_mem.cpp
    linux/rt_os_mutex.cpp
    linux/rt_os_thread.cpp
    linux/rt_os_time.cpp
)

set(RT_BASE_SRC
    rt_array_list.cpp
    rt_dequeue.cpp
    rt_hash_table.cpp
    rt_linked_list.cpp
    rt_mem.cpp
    rt_mem_service.cpp
    rt_log.cpp
    rt_string_utils.cpp
    rt_test.cpp
    rt_metadata.cpp
    ${RT_BASE_LINUX_SRC}
    ${RT_BUFFER_SRC}
)

add_library(${RT_BASE_STATIC} STATIC ${RT_BASE_SRC})
set_target_properties(${RT_BASE_STATIC} PROPERTIES FOLDER "rt_base")

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_base")
