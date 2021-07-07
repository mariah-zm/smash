#pragma once

#include "tokeniser.h"
#include "variables.h"

/**
 * Commands which are not recognised by the shell are redirected to this function. 
 * The function searches for the command binary in the system search path, builds a new 
 * array of command arguments from the tokens array, and calls the builenv function
 * to create the environment of smash-specific shell variables to be passed to the child process.
 * @param tokens the user input tokenised in an array
 * @param token_count the number of tokens in the tokens array
 * @param shell_variables the map of shell variables
 * @returns 0 on success, the respective error code otherwise
 */ 
int external(token_t *tokens, int token_count, shell_var *shell_variables);

/**
 * Executes external commands by forking and executing excve.
 * @param command the command binary 
 * @param args the command arguments (including the command itself) terminated with NULL 
 * @param env the smash-specific shell variable to be passed to the child
 */ 
void fork_plus_exec(char *command, char **args, char **env);


/**
 * Copies all shell varibales in a new array of strings. The variables are in the form of 
 * VAR_NAME=VAR_VALUE.
 * @param shell_variables the map containing the shell variables
 * @param count a pointer to the variable which will hold the total number of variables.
 * @returns a pointer to the allocated array of shell variables
 */ 
char **build_env(shell_var *shell_variables, int *count);

/**
 * Frees the memory allocated to new 2D array of shell variables passed to the child process.
 * @param env the array of strings
 * @param count the number of shell variables
 */ 
void destroy_env(char **env, int count);