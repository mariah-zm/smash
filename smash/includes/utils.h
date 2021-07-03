#pragma once

#include <stdbool.h>

// Input syntax patterns
#define QUOTATION_PATTERN "(^[^\"]*[\"][^\"]*[\"][^\"]*$)+"
#define VAR_NAME_PATTERN "^[a-zA-Z_][a-zA-Z0-9_]*$"
#define ASSIGNMENT_PATTERN "^[^=[:blank:]]+[=][^=[:blank:]].*$"

// Variable expansion patterns
#define ENCLOSED_PATTERN "^[{][^{}]+[}]$"
#define ENCLOSED_MORE_CHARS_PATTERN "^[{][^{}]+[}].+"
#define NOT_ENCLOSED_PATTERN "^[^{}\\s]+$"

#define MATCH 1
#define NOMATCH 0

int check_match(char* input, char* pattern);

void quote_removal(token_t tokens[MAX_NUM_TOKENS], int token_count);

bool is_assignment(char *string);

bool is_metachar_or_control_op(char *string);

void remove_char(char*string, char to_remove);

