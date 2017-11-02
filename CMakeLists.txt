cmake_minimum_required(VERSION 3.8)
project(mailserver)

set(CMAKE_CXX_STANDARD 11)
set(LINKER_FLAGS "-pthread -lldap -DLDAP_DEPRECATED")
set(COMPILE_FLAGS "-pthread -lldap -DLDAP_DEPRECATED")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMPILE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINKER_FLAGS}")

set(SOURCE_FILES_CLIENT client/client_main.cpp)
add_executable(client ${SOURCE_FILES_CLIENT})

set(SOURCE_FILES_SERVER server/server_main.cpp server/operation.h server/operations/operation_send.cpp server/client_handler.cpp server/client_handler.h server/operations/operation_send.h server/operations/operation_list.h server/operations/operation_read.h server/operations/operation_del.h server/operations/operation_read.cpp server/operations/operation_list.cpp server/operations/operation_del.cpp server/operation.cpp server/operations/operation_login.h server/operations/operation_login.cpp server/server.h server/server.cpp)
add_executable(server ${SOURCE_FILES_SERVER})

target_link_libraries(server /usr/lib/x86_64-linux-gnu/libldap.so)
target_link_libraries(server /usr/lib/x86_64-linux-gnu/liblber.so)