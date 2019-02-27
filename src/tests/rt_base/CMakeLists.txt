set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(RT_BASE_TEST_SRC
    rt_base_main.cpp
    test_base_array_list.cpp
    test_base_deque.cpp
    test_base_hash_table.cpp
    test_base_linked_list.cpp
    test_base_memory.cpp
    test_base_mutex_thread.cpp
    test_base_meta_data.cpp
)

add_executable(rt_base_test ${RT_BASE_TEST_SRC})
target_link_libraries(rt_base_test ${RT_BASE_STATIC} -pthread )

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config testcases for rt_base, [Done]")
