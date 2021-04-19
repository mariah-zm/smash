cmake_minimum_required(VERSION 3.15)
project(smash)

set(CMAKE_C_STANDARD 11)
include_directories(linenoise)

set(SOURCES linenoise.c)

add_executable(smash main.c ${SOURCES})