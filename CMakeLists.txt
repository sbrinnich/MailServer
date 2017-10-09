cmake_minimum_required(VERSION 3.8)
project(mailserver)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES_CLIENT main.cpp)
add_executable(client ${SOURCE_FILES_CLIENT})

set(SOURCE_FILES_SERVER server/server_main.cpp)
add_executable(server ${SOURCE_FILES_SERVER})