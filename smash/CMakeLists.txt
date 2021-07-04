cmake_minimum_required(VERSION 3.15)
project(smash)

set(CMAKE_C_STANDARD 11)
include_directories(linenoise includes)

set(UTILS linenoise/linenoise.c includes/impl/utils.c)
set(SMASH_SOURCES includes/impl/errors.c includes/impl/external.c includes/impl/internal.c includes/impl/redirection.c includes/impl/tokeniser.c includes/impl/variables.c)

add_executable(smash main.c ${UTILS} ${SMASH_SOURCES})