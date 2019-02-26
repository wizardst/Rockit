cmake_minimum_required( VERSION 2.8.8 )

include_directories(ff_node/include)
include_directories(rt_sink/include)

set(FF_NODE_SRC
    ff_node/FFNodeDecoder.cpp
    ff_node/FFNodeEncoder.cpp
    ff_node/FFNodeDemuxer.cpp
    ff_node/FFNodeFilter.cpp
    ff_node/FFNodeMuxer.cpp
)

set(RT_NODE_SRC
    RTNode.cpp
    RTNodeBus.cpp
    RTNodeCodec.cpp
    RTNodeDemuxer.cpp
    RTNodeFilter.cpp
    RTNodeMuxer.cpp
    RTNodeSink.cpp
    rt_node_define.cpp
    rt_sink/RTSinkAudioALSA.cpp
    rt_sink/RTNodeSinkAWindow.cpp
    rt_sink/RTNodeSinkGLES.cpp
    ${FF_NODE_SRC}
    hw_codec/HWNodeMpiDecoder.cpp
    hw_codec/HWNodeMpiEncoder.cpp
)


add_library(${RT_NODE_STATIC} STATIC ${RT_NODE_SRC})
add_definitions(-std=c++11)
set_target_properties(${RT_NODE_STATIC} PROPERTIES FOLDER "rt_node")
target_link_libraries(${RT_NODE_STATIC} ${RT_BASE_STATIC} ${RT_MEDIA_STATIC} -lffmpeg_4_0 -lmpp -lsink_osal)

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_node")
