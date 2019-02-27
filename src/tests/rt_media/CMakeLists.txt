cmake_minimum_required( VERSION 2.8.8 )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

if (HAVE_MPI)
    add_definitions(-DHAVE_MPI)
    set(MPI_CASES_SRC
        unit_test_display_gles.cpp
        unit_test_allocator.cpp
    )
    message(STATUS "build with MPI-Codec Support")
else()
    message(STATUS "build without MPI_CODEC_SRC")
endif()

set(RT_MEDIA_TEST_SRC
    rt_media_main.cpp
    unit_test_object_pool.cpp
    unit_test_ffmpeg_adapter.cpp
)

add_executable(rt_media_test ${RT_MEDIA_TEST_SRC} ${MPI_CASES_SRC})
target_link_libraries(rt_media_test ${RT_BASE_STATIC} ${RT_MEDIA_STATIC})
message(STATUS "cmake version ${CMAKE_VERSION} [@@]config testcases for rt_media, [Done]")
