cmake_minimum_required( VERSION 2.8.8 )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

include_directories(ff_node/include)
include_directories(rt_sink/include)

set(FF_NODE_SRC
    ff_node/FFNodeDecoder.cpp
    ff_node/FFNodeEncoder.cpp
    ff_node/FFNodeDemuxer.cpp
    ff_node/FFNodeFilter.cpp
    ff_node/FFNodeMuxer.cpp
)

if (HAVE_MPI)
    add_definitions(-DHAVE_MPI)
    set(MPI_CODEC_SRC
        hw_codec/HWNodeMpiDecoder.cpp
        hw_codec/HWNodeMpiEncoder.cpp
        rt_sink/RTNodeSinkAWindow.cpp
    )
    message(STATUS "build with MPI-Codec Support")
else()
    message(STATUS "build without MPI_CODEC_SRC")
endif()

set(RT_NODE_SRC
    RTNode.cpp
    RTNodeBus.cpp
    RTNodeCodec.cpp
    RTNodeDemuxer.cpp
    RTNodeFilter.cpp
    RTNodeMuxer.cpp
    RTNodeAudioSink.cpp
    rt_node_define.cpp
    rt_sink/RTSinkAudioALSA.cpp
    ${MPI_CODEC_SRC}
    ${FF_NODE_SRC}
    rt_sink/RTNodeSinkAWindow.cpp
    hw_codec/HWNodeMpiDecoder.cpp
    hw_codec/HWNodeMpiEncoder.cpp
)

if (HAVE_GLES)
    add_definitions(-DHAVE_GLES)
    set(GLES_FILES
            RTGLApp.cpp
            RTGLVideoScene.cpp
            RTGLView.cpp
            GLRender/MathMat4.cpp
            GLRender/MathVec4.cpp
            GLRender/RTAction.cpp
            GLRender/RTActionManager.cpp
            GLRender/RTCamera.cpp
            GLRender/RTDirector.cpp
            GLRender/RTScene.cpp
            GLRender/RTScheduler.cpp
            GLRender/RTShaderUtil.cpp
            GLRender/RTSprite.cpp
            GLRender/RTSpriteVideo.cpp)
    message(STATUS "build with GLES Support")
else()
    message(STATUS "build without GLES Support")
endif()

add_library(${RT_NODE_STATIC} STATIC ${RT_NODE_SRC})
add_definitions(-std=c++11)
set_target_properties(${RT_NODE_STATIC} PROPERTIES FOLDER "rt_node")
target_link_libraries(${RT_NODE_STATIC} ${RT_BASE_STATIC} ${RT_MEDIA_STATIC} -lmedia_osal)

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_node")
