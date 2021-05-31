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

#define MATCH 0
#define NOMATCH 1

int check_match(char* input, char* pattern);