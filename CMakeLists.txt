cmake_minimum_required(VERSION 3.7)

add_definitions(-std=c++17)

set(CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

project(Multithreaded_Server_Client_App)

add_executable(Server src/server.cpp)
add_executable(Client src/client.cpp)