#pragma once

#include <stdbool.h>

#include "tokeniser.h"
#include "variables.h"

#define ECHO 0
#define CD 1
#define SHOWVAR 2
#define EXPORT 3
#define UNSET 4
#define SHOWENV 5
#define PUSHD 6
#define POPD 7
#define DIRS 8
#define SOURCE 9
#define EXTERNAL 10

typedef struct dirnode {
    char *dir;
    struct dirnode *next_dir;
} dirnode;

/**
 * Determines which command the user wants to execute
 * @param command the command to be executed
 * @returns the value corersponding to the entered command (see macros above)
 */ 
int which_command(token_t command);

/**
 * Prints the arguments passed in the tokens array, or from the redirected input stream.
 * @param tokens the array containing the arguments to be printed
 * @param start the index of the first argument in the tokens array to be printed
 * @param end the index of the last argument in the tokens array to be printed
 */
void echo(token_t *tokens, int start, int end);

/**
 * Changes the current directory. The directory at the top of the stac kis replaaced with the new directory.
 * @param head a pointer to the head of the linked list which represents the top of the stack
 * @param directory the directory to change to
 * @returns true if the given directory exists and it the directttory was changed, false otherwise
 */
bool cd(dirnode **head, char *directory);

/**
 * Prints all the shell variables if the name is NULL, or the variable specified by the name. 
 * @param var_map the map of key-value pairs containing the shell variables
 * @param namevv the name of the shell variable to be printed
 */
void showvar(shell_var *var_map, char *name);

/**
 * Prints all the environment variables if the name is NULL, or the variable specified by the name. 
 * @param var_map the map of key-value pairs containing the shell variables
 * @param name the name of the environment variable to be printed
 */
void showenv(shell_var *var_map, char *name);

/**
 * Upgrades a shell variable to an environment variable.
 * @param var_map the map of key-value pairs containing the shell variables
 * @param name the name of the shell variable to be exported to an environment variable
 * @returns 0 on success, respective error code otherwise
 */
int export(shell_var *var_map, char *name);

/**
 * Unsets a shell variable by removing it from the map. If the variable is also an environment
 * variable, unsetenv is also called.
 * @param var_map the map of key-value pairs containing the shell variables
 * @param name the name of the shell variable to be unset
 * @returns 0 on success, respective error code otherwise
 */
int unset(shell_var *var_map, char *name);

/**
 * Creates a directory stack, implemented as a linked list, and initiliases it with the user's home directory.
 * @param home the user's home directory
 * @returns the allocated head of the directttory stack which corresponds to the to pof the stack
 */
dirnode *init_dir(char *home);

/**
 * Pushes a new directory onto the stack. When a directcory is pushed, the current directory
 * changes to mirror the new directory at the top of the stack. 
 * @param head a pointer to the head of the linked list which represents the top of the stack
 * @param directory the new directory to be pushed onto the stack
 * @returns true if the directory exists and is added, false otherwise
 */
bool pushdir(dirnode **head, char *directory);

/**
 * Pops the directory found at the top of the stack. When a directcory is popped, the current directory
 * changes to mirror the new directory at the top of the stack. 
 * @param head a pointer to the head of the linked list which represents the top of the stack
 */
void popdir(dirnode **head);

/**
 * Prints the directories in the directory stack. 
 * @param head the head of the linked list which represents the top of the stack
 */
void dirs(dirnode *head);

/**
 * Freess the allocated memory the linked list representing the directory stack.
 * @param head a pointer to the head of the list which represents the top of the stack
 */
void destroy_dirs(dirnode **head);