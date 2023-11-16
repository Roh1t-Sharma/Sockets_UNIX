cmake_minimum_required(VERSION 3.26)
project(Sockets C)

set(CMAKE_C_STANDARD 11)

add_executable(Sockets Server.c
        Client.c)
