#pragma once

#include <stdbool.h>

#define MAX_TOKEN_STRLEN 500
#define MAX_NUM_TOKENS 100

typedef char token_t[MAX_TOKEN_STRLEN];

/**
 * Separates tthe input string into a series of toekns recursively, using the shell metcharacters and control operations.
 * Each delimiter is associated with a value, i.e. the level. When the string has been tokenised w.r.t a certain delim, 
 * the function is called again with the next delim level. The base case is reached after tokenising with \t  completes.
 * @param tokens the final token array which will hold all the tokens
 * @param input the raw user input
 * @param index the next location of where to put a tken
 * @param delim_level the level which correeesponds to a particular delimiter
 */ 
void tokenise(token_t *tokens, char *input, int *index, int delim_level);

/**
 * @param delim_level a value associated with a delimiter 
 * @returns the delimiter associated with the given delimieter level.
 */ 
char get_delim(int delim_lev);

/**
 * Checks whether the given string contains the delimiter (checks if or escaped quoted).
 * @param delimiter the character (i.e. delimiter) to look for which will separate the string
 * @param string the string to be checked
 * @returns true if it contains the delimiter, false otherwise
 */ 
bool contains_delim(char delimiter, char *string);