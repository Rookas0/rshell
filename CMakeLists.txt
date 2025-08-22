cmake_minimum_required(VERSION 3.10)
project(rshell C)

set(CMAKE_C_STANDARD 11)

include_directories(
    src/term
    src/list
    src/tokenize
    src/readline
    src/exec
    src/exec/builtins
    src/parser
    src/util)

set(SOURCES
    src/shell.c
    src/term/term.c
    src/list/list.c
    src/tokenize/tokenize.c
    src/readline/readline.c
    src/exec/exec.c
    src/exec/builtins/builtins.c
    src/parser/parser.c
    src/util/util.c)

add_executable(rshell ${SOURCES})
