#include "dirstack.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

dirnode *init_dir(char *home){
    dirnode *head = (dirnode *) malloc(sizeof(dirnode));
    head->dir = (char *) malloc(sizeof(char) * strlen(home));

    strcpy(head->dir, home);
    head->next_dir = NULL;

    return head;
}

// passing ptr to head because we want to change the pointer ocntents
bool pushdir(dirnode **head, char *directory)
{   
    // if directory does not exist, do not push to stack
    if(chdir(directory) == 0){    
        dirnode *new_head = (dirnode *) malloc(sizeof(dirnode));
        new_head->dir = (char *) malloc(sizeof(char) * strlen(directory));
    
        strcpy(new_head->dir, directory);
        new_head->next_dir = *head;
        *head = new_head;

        return true;
    }

    return false;
}

void popdir(dirnode **head)
{
    dirnode *new_head = (*head)->next_dir;

    // stack must always contain home directory so we dont pop if end of linked list
    if(new_head != NULL){
        free(*head);
        *head = new_head;

        // Changing directory after pop
        chdir(new_head->dir);
    }
}

void dirs(dirnode *head)
{
    dirnode *current = head;

    while(current != NULL){
        printf("%s ", current->dir);

        current = current->next_dir;
    }

    printf("\n");
}