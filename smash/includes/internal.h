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

int which_command(token_t command);

void echo(token_t *tokens, int start, int end);

bool cd(dirnode **head, char *directory);

void showvar(shell_var *var_map, char *name);

void showenv(shell_var *var_map, char *name);

int export(shell_var *var_map, char *name);

int unset(shell_var *var_map, char *name);

dirnode *init_dir(char *home);

bool pushdir(dirnode **head, char *directory);

void popdir(dirnode **head);

void dirs(dirnode *head);

void destroy_dirs(dirnode **head);