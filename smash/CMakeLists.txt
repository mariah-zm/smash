cmake_minimum_required(VERSION 3.15)
project(smash)

set(CMAKE_C_STANDARD 11)
include_directories(linenoise includes)

set(UTILS linenoise/linenoise.c includes/impl/matcher.c)
set(SMASH_SOURCES includes/impl/parser.c includes/impl/errors.c includes/impl/variables.c includes/impl/executer.c includes/impl/dirstack.c includes/impl/redirection.c)

add_executable(smash main.c ${UTILS} ${SMASH_SOURCES})