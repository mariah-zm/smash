#pragma once

#define MAX_TOKEN_STRLEN 500
#define MAX_NUM_TOKENS 100
#define MAX_ENV_NAME_STRLEN 30
#define MAX_ENV_VALUE_STRLEN 200

#define PIPE "|"
#define GREATER_THAN ">"
#define SMALLER_THAN "<"
#define SEMICOLON ";"
#define SPACE " "
#define TAB "\t"
#define NEWLINE "\n"
#define QUOTATION "\""
#define DOLLAR_SIGN '$'
#define BACKSLASH "\\"

typedef char _token[MAX_TOKEN_STRLEN];

void parse(char* line);

int tokenise(char* line, _token tokens[MAX_NUM_TOKENS]);

