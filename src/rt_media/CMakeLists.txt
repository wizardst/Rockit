cmake_minimum_required( VERSION 2.8.8 )

include_directories(GLRender)
include_directories(FFMpeg)

set(RT_MEDIA_SRC
    RTGLApp.cpp
    RTGLVideoScene.cpp
    RTGLView.cpp
    RTMediaBuffer.cpp
    RTObject.cpp
    RTObjectPool.cpp
    FFMpeg/FFAdapterCodec.cpp
    FFMpeg/FFAdapterFilter.cpp
    FFMpeg/FFAdapterFormat.cpp
    FFMpeg/FFAdapterUtils.cpp
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
    GLRender/RTSpriteVideo.cpp
)

add_library(${RT_MEDIA_STATIC} STATIC ${RT_MEDIA_SRC})
set_target_properties(${RT_MEDIA_STATIC} PROPERTIES FOLDER "rt_media")
target_link_libraries(${RT_MEDIA_STATIC} ${RT_BASE_STATIC})

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_media")
