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

shell_var *init_shell_vars(char *shell, char **envp);

void update_cwd(shell_var *var_map, char *dir);

int insert_shell_var(shell_var *var_map, char *name, char *value, bool is_env);

int update_shell_var(shell_var *var_map, int index, char *value);

int remove_shell_var(shell_var *var_map, char *name);

char *get_shell_var(shell_var *var_map, char *name);

int get_hashcode(char *name);

int export_shell_var(shell_var *var_map, char *name);

int expand_var(shell_var *var_map, char *input, char *result);

bool is_var_name_char(char character);

int var_assignment(char *assignment, char *var_name, char *var_value);

bool is_var_name_valid(char *name);

void showvar(shell_var *var_map, char *name);

void showenv(shell_var *var_map, char *name);

void destroy_shell_vars(shell_var *var_map);