#pragma once

#include <stdbool.h>

#include "tokeniser.h"

void parse(char *line);

int variable_expansion(token_t tokens[MAX_NUM_TOKENS], int token_count);

int expand(char* string);

void quote_removal(token_t tokens[MAX_NUM_TOKENS], int token_count);

bool is_assignment(char *string);

bool is_metachar_or_control_op(char *string);

