cmake_minimum_required(VERSION 3.10)
project(rshell C)

set(CMAKE_C_STANDARD 11)

include_directories(
    src/term
    src/list)

set(SOURCES
    src/shell.c
    src/term/term.c
    src/list/list.c)

add_executable(rshell ${SOURCES})
