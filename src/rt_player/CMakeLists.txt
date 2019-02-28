cmake_minimum_required( VERSION 2.8.8 )

include_directories(./)

set(RT_PLAYER_SRC
    RTNDKMediaPlayer.cpp
    RTNDKIPTVPlayer.cpp
)

add_library(${RT_PLAYER_STATIC} STATIC ${RT_PLAYER_SRC})
add_definitions(-std=c++11)
set_target_properties(${RT_PLAYER_STATIC} PROPERTIES FOLDER "rt_player")
target_link_libraries(${RT_PLAYER_STATIC} ${RT_BASE_STATIC} ${RT_MEDIA_STATIC} ${RT_NODE_STATIC} ${RT_TASK_STATIC} -lffmpeg_4_0 -lmpp -lsink_osal)

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_player")
