cmake_minimum_required( VERSION 2.8.8 )

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

include_directories(.)

# ----------------------------------------------------------------------------
# mpp built-in unit test case
# ----------------------------------------------------------------------------
# macro for adding mpp sub-module unit test
macro(add_rockit_test module)
    set(test_name ${module})
    string(TOUPPER ${test_name} test_tag)
    #message(STATUS "moduule   : ${module}")
    #message(STATUS "test_name : ${test_name}")
    #message(STATUS "test_tag  : ${test_tag}")

    option(${test_tag} "Build rockit ${module} unit test" ON)
    if(${test_tag})
        add_executable(${test_name} ${test_name}.cpp player_test_utils.cpp)
        target_link_libraries(${test_name} ${RT_BASE_STATIC} ${RT_PLAYER_STATIC} ${RT_MEDIA_STATIC} ${RT_NODE_STATIC} ${RT_TASK_STATIC} -lasound -pthread ${MPI_LIBS})
        set_target_properties(${test_name} PROPERTIES FOLDER "test")
        #add_test(NAME ${test_name} COMMAND ${test_name})
    endif()
endmacro()

add_rockit_test(case_player_std)

add_rockit_test(case_player_rand)
