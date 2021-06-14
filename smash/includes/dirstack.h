#pragma once

#include <stdbool.h>

typedef struct dirnode
{
    char *dir;
    struct dirnode *next_dir;
} dirnode;

dirnode *init_dir(char *home);

bool pushdir(dirnode **head, char *directory);

void popdir(dirnode **head);

void dirs(dirnode *head);