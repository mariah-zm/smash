#pragma once

#include "parser.h"
#include "dirstack.h"

#define EXIT 0
#define ECHO 1
#define CD 2
#define SHOWVAR 3
#define EXPORT 4
#define UNSET 5
#define SHOWENV 6
#define PUSHD 7
#define POPD 8
#define DIRS 9
#define SOURCE 10
#define EXTERNAL 11

void execute(token_t *tokens, int start, int end, dirnode *head, shell_var *shell_variables);

void execute_source(char *file_name, dirnode *head, shell_var *shell_variables);

void fork_plus_exec(char *command, char **args, char **env);

char **build_env(shell_var *shell_variables, int *count);

void destroy_env(char **env, int count);

int which_command(token_t command);

void echo(token_t *tokens, int start, int end);

int external(token_t *tokens, int token_count, shell_var *shell_variables);

bool cd(dirnode **head, char *directory);