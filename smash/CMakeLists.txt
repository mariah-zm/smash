cmake_minimum_required(VERSION 3.15)
project(smash)

set(CMAKE_C_STANDARD 11)
include_directories(linenoise includes)

set(UTILS linenoise/linenoise.c includes/matcher.c)
set(SMASH_SOURCES includes/parser.c includes/errors.c includes/variables.c includes/tokeniser.c includes/executer.c includes/dirstack.c)

add_executable(smash main.c ${UTILS} ${SMASH_SOURCES})