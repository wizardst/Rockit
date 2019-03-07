cmake_minimum_required( VERSION 2.8.8 )

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

include_directories(.)

set(RT_PLAYER_TEST_SRC
    RTPairAudioPlayer.cpp
    RTPairAudioCases.cpp
    main.cpp
)

add_executable(rt_player_test ${RT_PLAYER_TEST_SRC})
target_link_libraries(rt_player_test ${RT_BASE_STATIC} ${RT_MEDIA_STATIC} ${RT_NODE_STATIC} ${RT_TASK_STATIC} ${RT_PLAYER_STATIC})
message(STATUS "cmake version ${CMAKE_VERSION} [@@]config testcases for rt_player, [Done]")