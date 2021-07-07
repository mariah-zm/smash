#pragma once 

#include <stdbool.h>

#define MAX_VAR_NAME_STRLEN 100
#define MAX_VAR_VALUE_STRLEN 1000
#define MAX_VARIABLES 200

#define PROMPT "smash> "

typedef struct {
    char name[MAX_VAR_NAME_STRLEN];
    char value[MAX_VAR_VALUE_STRLEN];
    bool is_env;
} shell_var;

/**
 * Allocates memory to the hash map to contain the shell variables. Initialises it with the smash-specific
 * variables TERMINAL, PATH etc, and also with the environment variables passed to the main function 
 * on shell start up.
 * @param shell the nam, which is the full path, of the program (i.e. of the shell) 
 * @param envp an array of pointers to the environment variables
 * @returns the allocated map of key-value pairs corresponding to the shell variables
 */  
shell_var *init_shell_vars(char *shell, char **envp);

/**
 * Expands all the variables found in input. 
 * @param var_map the map of key-value pairs containing the shell variables
 * @param input the input containing variables to be expanded
 * @param result the result withh all variables expanded
 * @returns 0 on succes, the ressepctive error code otherwise
 */
int expand_var(shell_var *var_map, char *input, char *result);

/**
 * Updates the values of the variables CWD and PWD with the given directory path
 * @param var_map the map of key-value pairs containing the shell variables
 * @param dir the directory to update to
 */
void update_cwd(shell_var *var_map, char *dir);

/**
 * Takes a character and returnns whether it is a valid variable name character (a-z, A-Z, _, or 0-9)
 * @param character the character to be checked
 * @returns true if a valid variable name character, false otherwise
 */
bool is_var_name_char(char character);

/**
 * Variable assignment creates a new shell variable or updates an existing.
 * @param var_map the map of key-value pairs containing the shell variables
 * @param assignment the variable assignment input in the form of VAR_NAME=VAR_VALUE
 * @returns 0 on success, respective error code otherwise
 */
int var_assignment(shell_var *var_map, char *assignment);

/**
 * Checks whether a variable name is valid.
 * @param name the variable name to be validated
 * @returns true if valid, false otherwise
 */
bool is_var_name_valid(char *name);

/**
 * Inserts keys and their corresponding values in the map. Open addressing is used, and 
 * so the map is probed until a free space is found.
 * @param var_map the map of key-value pairs containing the shell variables
 * @param name the variable name which must be unique, i.e. the key
 * @param value the variable value
 * @param is_env true if the shell variable is also an environment variable, false otherwise
 * @returns 0 on succes, the ressepctive error code otherwise
 */
int insert_shell_var(shell_var *var_map, char *name, char *value, bool is_env);

/**
 * Updates the value of the shell variable at the location specified by index.
 * @param var_map the map of key-value pairs containing the shell variables
 * @param index the index of the key-value pair in the map
 * @param value the value to update to
 * @returns 0 on succes, the ressepctive error code otherwise
 */
int update_shell_var(shell_var *var_map, int index, char *value);

/**
 * Searches the var_map by first computing the hash of the key (i.e. the name) to get the index
 * of the variable in the map. If the variable is not found at that index, a linear search is performed
 * from that point onwards until the variable is found or an empty variable is found (in which 
 * case the variable does not exist in the map).
 * @param var_map the map of key-value pairs containing the shell variables
 * @param name the variable name identifying the value
 * @returns a pointer to the value if found, NULL otherwise
 */
char *get_shell_var(shell_var *var_map, char *name);

/**
 * Maps the key, i.e. the name, to a value using a hash function.
 * @param name the variable name used as a key in the map
 * @returns the hash corresponding to the variable name
 */
int get_hashcode(char *name);

/**
 * Frees the memroy allocated to the map of shell variables.
 * @param var_map the map of key-value pairs containing the shell variables
 */
void destroy_shell_vars(shell_var *var_map);