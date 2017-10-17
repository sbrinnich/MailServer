cmake_minimum_required(VERSION 3.8)
project(mailserver)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES_CLIENT client/client_main.cpp)
add_executable(client ${SOURCE_FILES_CLIENT})

set(SOURCE_FILES_SERVER server/server_main.cpp server/operation.h server/operations/operation_send.cpp server/client_handler.cpp server/client_handler.h server/operations/operation_send.h server/operations/operation_list.h server/operations/operation_read.h server/operations/operation_del.h server/operations/operation_read.cpp server/operations/operation_list.cpp server/operations/operation_del.cpp server/operation.cpp)
add_executable(server ${SOURCE_FILES_SERVER})