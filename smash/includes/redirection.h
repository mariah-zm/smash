#pragma once

#include <stdbool.h>

#include "tokeniser.h"

#define STDIN 0
#define STDOUT 1

#define OVERWRITE 0
#define APPEND 1

/**
 * Resets the given stream to the standard stream.
 * @stream STDIN or STDOUT (see macros above)
 */ 
void reset(int stream);

/**
 * Redirects the input by opening the file descriptor corresponding to the given file found in tokens
 * and duplicating it to saved stdin.
 * @param tokens the array of tokens containing the file/stream name
 * @param in_pos the index of '<' in the tokens array
 * @returns 0 on success, the respective error code otherwise
 */ 
int redir_in(token_t *tokens, int in_pos);

/**
 * Redirects the output by opening the file descriptor corresponding to the given file found in tokens
 * and duplicating it to saved stdout.
 * @param tokens the array of tokens containing the file/stream name
 * @param out_pos the index of '>' in the tokens array
 * @param write_flag OVERWRITE or APPEND (see macros above)
 * @returns 0 on success, the respective error code otherwise
 */ 
int redir_out(token_t *tokens, int out_pos, int write_flag);

/**
 * @returns true if the input has been redirected to a new stream
 */ 
bool is_inredir(void);