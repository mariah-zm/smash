#pragma once

#include <stdbool.h>

#include "tokeniser.h"

#define STDIN 0
#define STDOUT 1

#define OVERWRITE 0
#define APPEND 1

void reset(int stream);

int redir_in(token_t *tokens, int in_pos);

int redir_out(token_t *tokens, int out_pos, int write_flag);

bool is_inredir(void);