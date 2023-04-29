cmake_minimum_required(VERSION 3.10)

project(socks C)

set(CMAKE_C_STANDARD 11)

add_library(socks_objects OBJECT "src/socks_init.c" "src/socks_close.c" "src/socks_log.c" "src/socks_connect.c" "src/socket_config.c" "src/thread.c" "src/socks_accept.c" "src/socks_accept.c" "src/socks_listen.c" "src/sendToSocket.c" "src/readFromSocket.c")
target_compile_options(socks_objects PRIVATE "-fpic" "-g")

add_library(libsocks-shared SHARED $<TARGET_OBJECTS:socks_objects>)
target_link_libraries(libsocks-shared "pthread")
add_library(libsocks-static STATIC  $<TARGET_OBJECTS:socks_objects>)
target_link_libraries(libsocks-static "pthread")


add_executable(server "src/test/server.c" $<TARGET_OBJECTS:socks_objects>)
target_link_libraries(server "pthread")
target_compile_options(server PRIVATE "-g")

add_executable(client "src/test/client.c" $<TARGET_OBJECTS:socks_objects>)
target_link_libraries(client "pthread")
target_compile_options(client PRIVATE "-g")

