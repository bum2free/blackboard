cmake_minimum_required(VERSION 3.10)
project(test_blackboard)

# enable C++17 standard
set(CMAKE_CXX_STANDARD 17)

# enable debug
#set(CMAKE_BUILD_TYPE Debug)
# Add O3 optimization
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(SRC_FILES
    srcs/main.cpp
    srcs/payload.cpp
    srcs/cpu_load.cpp
    srcs/writer.cpp
    srcs/reader.cpp
    srcs/statistics.cpp
    dummy_blackboard/blackboard.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
set(SRC_FILES ${SRC_FILES}
    behavior_tree/main.cpp
    behavior_tree/writer.cpp
    behavior_tree/reader.cpp
    cyber/main.cpp
    cyber/writer.cpp
    cyber/reader.cpp
)
endif()

add_executable(test_blackboard ${SRC_FILES})

target_include_directories(test_blackboard PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/
    ${PROJECT_SOURCE_DIR}/../../thirdparty/common-algorithm/common_noa/thirdparty/json
    ${PROJECT_SOURCE_DIR}/../../thirdparty/avp-slam/third_party/pangolin/utils/xml
    ${PROJECT_SOURCE_DIR}/../../thirdparty/common-algorithm/behavior_tree/Groot/depend/BehaviorTree.CPP/include
)

# if CMAKE_SYSTEM_NAME is QNX
if(CMAKE_SYSTEM_NAME STREQUAL "QNX")
    #    target_link_libraries(test_blackboard c ${SOCKET_LIB})
    target_link_libraries(test_blackboard behaviortree_cpp_v3)
    target_link_libraries(test_blackboard ${LIBCYBER_LIBRARY})
    target_link_libraries(test_blackboard protobuf_)
    target_link_libraries(test_blackboard fastrtps)
    target_link_libraries(test_blackboard glog::glog)
    target_link_libraries(test_blackboard ${LIBUUID_LIBRARY})
endif()

# set compile flag USE_THREAD_TIMER
add_definitions(-DUSE_THREAD_TIMER)
