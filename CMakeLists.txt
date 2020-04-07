cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

project(Multithreaded_Server_Client_App)

add_executable(Server src/server.cpp src/resource.cpp src/resource_manager.cpp)
add_executable(Client src/client.cpp)