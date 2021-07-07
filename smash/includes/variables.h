#pragma once 

#include <stdbool.h>

#define MAX_VAR_NAME_STRLEN 100
#define MAX_VAR_VALUE_STRLEN 1000
#define MAX_VARIABLES 200

#define PROMPT "smash> "

typedef struct
{
    char *name;
    char *value;
    bool is_env;
}shell_var;

/**
 * @param shell
 * @param envp
 * @returns the allocated map of key-value pairs corresponding to the shell variables
 */  
shell_var *init_shell_vars(char *shell, char **envp);

/**
 * @param var_map
 * @param dir
 */
void update_cwd(shell_var *var_map, char *dir);

/**
 * @param var_map
 * @param name
 * @param value
 * @param is_env true if the shell variable is also an environment variable, false otherwise
 * @returns 0 on succes, the ressepctive error code otherwise
 */
int insert_shell_var(shell_var *var_map, char *name, char *value, bool is_env);

/**
 * @param var_map 
 * @param index
 * @param value
 * @returns 0 on succes, the ressepctive error code otherwise
 */
int update_shell_var(shell_var *var_map, int index, char *value);

/**
 * @param var_map
 * @param name
 * @returns
 */
char *get_shell_var(shell_var *var_map, char *name);

/**
 * @param name the varibale name used as a key in the map
 * @returns the hash corresponding to the variable name
 */
int get_hashcode(char *name);

/**
 * @param var_map 
 * @param input
 * @param result
 * @returns 0 on succes, the ressepctive error code otherwise
 */
int expand_var(shell_var *var_map, char *input, char *result);

/**
 * @param character
 * @returns true if valid variable name character, false otherwise
 */
bool is_var_name_char(char character);

/**
 * @param assignment
 * @param var_name
 * @param var_value
 * @returns 0 on success, respective error code otherwise
 */
int var_assignment(char *assignment, char *var_name, char *var_value);

/**
 * @param name
 * @returns true if valid, false otherwise
 */
bool is_var_name_valid(char *name);

/**
 * Frees the memroy allocated to the map of shell variables.
 * @param var_map
 */
void destroy_shell_vars(shell_var *var_map);