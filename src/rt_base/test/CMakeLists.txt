set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

set(RT_BASE_TEST_SRC
    main.cpp
)

add_executable(rt_base_test ${RT_BASE_TEST_SRC})
target_link_libraries(rt_base_test ${RT_BASE_STATIC})

message(STATUS "cmake version ${CMAKE_VERSION}")
