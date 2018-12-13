cmake_minimum_required( VERSION 2.8.8 )

include_directories(ff_node/include)

set(RT_NODE_SRC
    rt_node.cpp
    rt_node_bus.cpp
    rt_node_parser.cpp
    rt_node_define.cpp

    ff_node/ff_node_codec.cpp
    ff_node/ff_node_parser.cpp
    ff_node/ffmpeg_wrapper.cpp
)


add_library(${RT_NODE_STATIC} STATIC ${RT_NODE_SRC})
set_target_properties(${RT_NODE_STATIC} PROPERTIES FOLDER "rt_node")
target_link_libraries(${RT_NODE_STATIC} ${RT_BASE_STATIC} -lffmpeg_4_0)

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_node")
