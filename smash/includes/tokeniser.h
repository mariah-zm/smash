#pragma once

#define MAX_TOKEN_STRLEN 500
#define MAX_NUM_TOKENS 100

#define PIPE '|'
#define GREATER_THAN '>'
#define SMALLER_THAN '<'
#define SEMICOLON ';'
#define SPACE ' '
#define TAB '\t'
#define NEWLINE '\n'
#define QUOTATION '\"'
#define DOLLAR_SIGN '$'
#define BACKSLASH '\\'

typedef char token_t[MAX_TOKEN_STRLEN];

int tokenise(char* line);