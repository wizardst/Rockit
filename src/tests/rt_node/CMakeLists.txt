set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(RT_NODE_TEST_SRC
    rt_node_main.cpp
    test_node_basic.cpp
    test_node_bus.cpp
    test_node_data_flow.cpp
)

add_executable(rt_node_test ${RT_NODE_TEST_SRC})
target_link_libraries(rt_node_test ${RT_BASE_STATIC})

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config testcases for rt_node")
