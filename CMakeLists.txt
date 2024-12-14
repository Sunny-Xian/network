cmake_minimum_required(VERSION 3.28)
project(network)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads)

set(CMAKE_CXX_FLAGS "${CAMKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(demo1 demo1.cpp)
add_executable(demo2 demo2.cpp)
add_executable(demo3 demo3.cpp)
add_executable(demo4 demo4.cpp)
add_executable(demo5 demo5.cpp)
add_executable(demo6 demo6.cpp)
add_executable(demo7 demo7.cpp)
add_executable(demo8 demo8.cpp)
add_executable(demo10 demo10.cpp)
add_executable(demo11 demo11.cpp)
add_executable(demo12 demo12.cpp)
add_executable(tcpselect tcpselect.cpp)
add_executable(client client.cpp)
add_executable(tcppoll tcppoll.cpp)
add_executable(tcpepoll tcpepoll.cpp)

add_executable(abcdef abcdef.cpp)
add_executable(test001 test001.cpp)
target_link_libraries(test001 ${CMAKE_THREAD_LIBS_INIT})