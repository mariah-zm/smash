#pragma once

#include <stdbool.h>

#define MAX_TOKEN_STRLEN 500
#define MAX_NUM_TOKENS 100

typedef char token_t[MAX_TOKEN_STRLEN];

void tokenise(token_t *tokens, char* input, int* index, int delim_level);

char get_delim(int delim_lev);

bool contains_delim(char delimeter, char* string);

bool is_quoted(char* full_string, char* sub_string);

bool is_escaped(char* ptr);
