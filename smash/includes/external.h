#pragma once

#include "tokeniser.h"
#include "variables.h"

int external(token_t *tokens, int token_count, shell_var *shell_variables);

void fork_plus_exec(char *command, char **args, char **env);

char **build_env(shell_var *shell_variables, int *count);

void destroy_env(char **env, int count);
