#pragma once 

#define VAR_NAME_STRLEN 50



/**
 * Populates the environment variables on application start up. 
 * 
 * @return 0 if successful, specific 
 */ 
int init_shell_vars();

/**
 * Performs variable expansion.
 * 
 * @param var the variable to be expanded
 * @param expanded the destination of the value of the expanded variable
 * @return
 */ 
int expand_var(char* var, char* expanded);

/**
 * Modifies a shell variable or creates a new one if it does not exist.
 * 
 * @param statement an assignment statement in the form VARIABLE=VALUE 
 * @return
 */ 
int set_shell_var(char* statement);

void add_var(char* name);