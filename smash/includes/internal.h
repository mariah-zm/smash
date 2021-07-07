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
 * @param command
 * @returns
 */ 
int which_command(token_t command);

/**
 * @param tokens
 * @param start
 * @param end
 */
void echo(token_t *tokens, int start, int end);

/**
 * 
 */
bool cd(dirnode **head, char *directory);

/**
 * @param var_map
 * @param namevv
 */
void showvar(shell_var *var_map, char *name);

/**
 * @param var_map
 * @param name
 */
void showenv(shell_var *var_map, char *name);

/**
 * @param var_map
 * @param name
 * @returns 0 on success, respective error code otherwise
 */
int export(shell_var *var_map, char *name);

/**
 * @param var_map
 * @param name
 * @returns 0 on success, respective error code otherwise
 */
int unset(shell_var *var_map, char *name);

/**
 * @param home
 * @returns 
 */
dirnode *init_dir(char *home);

/**
 * @param head a pointer to the head of the linked list which represents the top of the stack
 * @param directory the new directory to be pushed onto the stack
 * @returns true if the directory exists and is added, false otherwise
 */
bool pushdir(dirnode **head, char *directory);

/**
 * @param head a pointer to the head of the linked list which represents the top of the stack
 */
void popdir(dirnode **head);

/**
 * @param head the head of the linked list which represents the top of the stack
 */
void dirs(dirnode *head);

/**
 * Freess the allocated memory the linked list representing the directory stack.
 * @param head a pointer to the head of the list which represents the top of the stack
 */
void destroy_dirs(dirnode **head);