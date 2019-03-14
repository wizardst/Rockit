cmake_minimum_required( VERSION 2.8.8 )

include_directories(./)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(RT_PLAYER_SRC
    RTNDKMediaPlayer.cpp
    RTNDKIPTVPlayer.cpp
    RTNDKNodePlayer.cpp
    RTNDKMediaDef.cpp
    RockitPlayer.cpp
)

add_library(${RT_PLAYER_STATIC} STATIC ${RT_PLAYER_SRC})
add_definitions(-std=c++11)
set_target_properties(${RT_PLAYER_STATIC} PROPERTIES FOLDER "rt_player")
target_link_libraries(${RT_PLAYER_STATIC} ${RT_BASE_STATIC} ${RT_MEDIA_STATIC} ${RT_NODE_STATIC} ${RT_TASK_STATIC})

add_library(${RT_PLAYER_SHARED} SHARED ${RT_PLAYER_SRC})
add_definitions(-std=c++11)
set_target_properties(${RT_PLAYER_SHARED} PROPERTIES FOLDER "rt_player")
target_link_libraries(${RT_PLAYER_SHARED} ${RT_BASE_STATIC} ${RT_MEDIA_STATIC} ${RT_NODE_STATIC} ${RT_TASK_STATIC})

message(STATUS "cmake version ${CMAKE_VERSION} [@@]config library: rt_player")
