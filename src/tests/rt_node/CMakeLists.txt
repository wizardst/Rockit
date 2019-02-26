set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(RT_NODE_TEST_SRC
    rt_node_main.cpp
    test_node_bus.cpp
    test_node_data_flow.cpp
    test_node_codec_with_gles.cpp
    test_node_codec_with_render.cpp
    test_node_ffmpeg_demuxer.cpp
    test_node_audio_codec.cpp
    test_node_simple_player.cpp
)

add_executable(rt_node_test ${RT_NODE_TEST_SRC})
target_link_libraries(rt_node_test ${RT_BASE_STATIC} ${RT_NODE_STATIC} ${RT_TASK_STATIC})

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config testcases for rt_node")
