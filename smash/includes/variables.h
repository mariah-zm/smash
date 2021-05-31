#pragma once 

#include <stdbool.h>

#define MAX_VAR_NAME_STRLEN 100
#define MAX_VAR_VALUE_STRLEN 1000
#define MAX_VARIABLES 200

#define PROMPT "smash> "

typedef struct{
    char *name;
    char *value;
    bool is_env;
}shell_var;

/**
 * Populates the environment variables on application start up. 
 * 
 * @return 0 if successful, specific 
 */ 
int init_shell_vars(char *shell, char **envp);

int insert_shell_var(char *name, char *value, bool is_env);

int update_shell_var(int index, char *name, char *value, bool is_env);

int remove_shell_var(char *name);

char *get_shell_var(char *name);

int get_hashcode(char *name);

/**
 * Performs variable expansion.
 * 
 * @param var the variable to be expanded
 * @param expanded the destination of the value of the expanded variable
 * @return
 */ 
int expand_var(char *var, char *expanded);

/**
 * Modifies a shell variable or creates a new one if it does not exist.
 * 
 * @param statement an assignment statement in the form VARIABLE=VALUE 
 * @return OK if 
 */ 
int var_assignment(char *assignment, char *var_name, char *var_value);

bool is_var_name_valid(char *name);

void showvar(char *name);

void destroy_shell_vars();