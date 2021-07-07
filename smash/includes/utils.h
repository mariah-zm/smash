#pragma once

#include <stdbool.h>

#include "tokeniser.h"

// Input syntax patterns
#define QUOTATION_PATTERN "([^\"]*[\"][^\"]*[\"][^\"]*)+"
#define VAR_NAME_PATTERN "^[a-zA-Z_][a-zA-Z0-9_]*$"
#define ASSIGNMENT_PATTERN "^[^=[:blank:]]+[=][^=[:blank:]].*$"
#define IS_CHAR_QUOTED_PATTERN "^[^\"]*[\"][^\"]*([^\"]*[\"][^\"]*[\"])*$"

// Variable expansion patterns
#define ENCLOSED_PATTERN "^[{][^{}]+[}]$"
#define ENCLOSED_MORE_CHARS_PATTERN "^[{][^{}]+[}].+"
#define NOT_ENCLOSED_PATTERN "^[^{}\\s]+$"

#define MATCH 1
#define NOMATCH 0

/**
 * Converts the given pattern to regex and matches the given string to it.
 * @param input the string to be compared against the pattern
 * @param pattern the pattern to be compiled as regex
 * @returns 1 if it matches the regex, 0 if not, -1 if regex does not compile
 */ 
int check_match(char *input, char *pattern);

/**
 * Removes escape caharcters and double quotes from each token.
 * @param tokens the input tokenised in a 2d array
 * @param token_count the number of non-empty tokens in the array
 */ 
void quote_removal(token_t *tokens, int token_count);

/**
 * Removes all occurences of the given character in the token recursively.
 * @param token the string from which to remove the character
 * @param to_remove the character to be removed
 */ 
void remove_char(token_t token, char to_remove);

/**
 * Checks whether a given substring is surrounded by quotes in the full string.
 * @param full_string the complete string
 * @param sub_string a part of the complete string
 * @returns true if the sub-string is found within quotes, false otherwise
 */ 
bool is_quoted(char *full_string, char *sub_string);


/**
 * Checks whether the first character of the given string is preceeded by a backslash, i.e. is escaped.
 * @param ptr the string of which the first character needs to be checked if it is escaped
 * @returns true if it is escaped, false otherwise
 */ 
bool is_escaped(char *ptr);

/**
 * Checks whether the input is a variable assignment. The function uses the ASSIGNMENT_PATTERN
 * to match the token with the regex. 
 * @param token input to be checked
 * @returns true if the token is a variable assignment, false otherwise
 */ 
bool is_assignment(token_t token);

/**
 * Checks whether the token is one of the following: ; | < >
 * @param token input to be checked
 * @returns true if the token is one of those characters, false otherwise
 */ 
bool is_metachar(token_t token);

/**
 * Returns the position in the array of the given symbol.
 * @param tokens the array of tokens to be searched
 * @param token_count the number of non-empty tokens in the array
 * @param symbol the single-character string to be searched for
 * @returns the positionin the array, or -1 if not found
 */ 
int get_pos(token_t *tokens, int token_count, char *symbol);