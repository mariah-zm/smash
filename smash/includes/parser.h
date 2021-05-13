#pragma once

#include <stdbool.h>

#define MAX_ENV_NAME_STRLEN 30
#define MAX_ENV_VALUE_STRLEN 200

void parse(char* line);

bool contains_metachar(char metachar, char* string);

bool is_quoted(char* full_string, char delim);

bool is_escaped(char* ptr);

void quote_removal(char* string, char to_remove);



