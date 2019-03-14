cmake_minimum_required( VERSION 2.8.8 )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

include_directories(GLRender)
include_directories(FFMpeg)
include_directories(Mpi/include)

if (HAVE_MPI)
    add_definitions(-DHAVE_MPI)
    set(MPI_FILES
            Mpi/MpiAdapterUtils.cpp
            Mpi/MpiAdapterCodec.cpp)
    message(STATUS "build with RK-MPI Support")
else()
    message(STATUS "build without RK-MPI Support")
endif()

if (HAVE_GLES)
    add_definitions(-DHAVE_GLES)
    set(GLES_FILES
            RTGLApp.cpp
            RTGLView.cpp
            RTGLVideoScene.cpp
            GLRender/MathMat4.cpp
            GLRender/RTAction.cpp
            GLRender/RTCamera.cpp
            GLRender/RTScene.cpp
            GLRender/RTShaderUtil.cpp
            GLRender/RTSpriteVideo.cpp
            GLRender/MathVec4.cpp
            GLRender/RTActionManager.cpp
            GLRender/RTDirector.cpp
            GLRender/RTScheduler.cpp
            GLRender/RTSprite.cpp
    )
    message(STATUS "build with GLES-EGL Support")
else()
    message(STATUS "build without GLES-EGL Support")
endif()

set(RT_MEDIA_SRC
    RTMediaBuffer.cpp
    RTMediaData.cpp
    RTObject.cpp
    RTObjectPool.cpp
    RTMediaBufferPool.cpp
    FFMpeg/FFAdapterCodec.cpp
    FFMpeg/FFAdapterFilter.cpp
    FFMpeg/FFAdapterFormat.cpp
    FFMpeg/FFAdapterUtils.cpp
    ${GLES_FILES}
    RTAllocator/RTAllocatorBase.cpp
    RTAllocator/RTAllocatorMalloc.cpp
    RTAllocator/RTAllocatorDrm.cpp
    RTAllocator/RTAllocatorIon.cpp
    RTAllocator/RTAllocatorStore.cpp
    ${MPI_FILES}
    ALSALib/ALSAAapterImpl.cpp
    ALSALib/ALSAAapterUtils.cpp
    ALSALib/ALSAVolumeManager.cpp
)

if (FFMPEG_SINGLE)
    set( FFMPEG_LIBS -lffmpeg_58 )
else()
    set( FFMPEG_LIBS
        ${LINUX_SYSROOT}/usr/lib/libavformat.so
        ${LINUX_SYSROOT}/usr/lib/libavutil.so
        ${LINUX_SYSROOT}/usr/lib/libavcodec.so
        ${LINUX_SYSROOT}/usr/lib/libswresample.so
    )
    # include_directories(${LINUX_SDK_ROOT}/buildroot/output/rockchip_rk3308_release/target/usr/include)
endif()

if (HAVE_MPI)
    set( MPI_LIBS -lmpp -lsink_osal)
    message(STATUS "build with RK-MPI Support")
else()
    message(STATUS "build without RK-MPI Support")
endif()

add_library(${RT_MEDIA_STATIC} STATIC ${RT_MEDIA_SRC})
set_target_properties(${RT_MEDIA_STATIC} PROPERTIES FOLDER "rt_media")
target_link_libraries(${RT_MEDIA_STATIC} ${RT_BASE_STATIC} -lasound -pthread ${MPI_LIBS} ${FFMPEG_LIBS})

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_media")
