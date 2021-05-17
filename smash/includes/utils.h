#pragma once

#include <stdbool.h>

// Input syntax patterns
#define QUOTATION_PATTERN "(^[^\"]*[\"][^\"]*[\"][^\"]*$)+"
#define VAR_NAME_PATTERN "^[a-zA-Z_][a-zA-Z0-9_]*$"
#define ASSIGNMENT_PATTERN "^[^= ]+[=][^= ]+$"

// Variable expansion patterns
#define ENCLOSED_PATTERN "^[{][^{}]+[}]$"
#define ENCLOSED_MORE_CHARS_PATTERN "^[{][^{}]+[}].+"
#define NOT_ENCLOSED_PATTERN "^[^{} ]+$"

bool is_matched(char* input, char* pattern);