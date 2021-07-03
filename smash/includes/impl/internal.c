#include "../internal.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int which_command(token_t command)
{
    if(strcmp(command, "echo") == 0)
        return ECHO;
    else if(strcmp(command, "cd") == 0)
        return CD;
    else if(strcmp(command, "showvar") == 0)
        return SHOWVAR;
    else if(strcmp(command, "export") == 0)
        return EXPORT;
    else if(strcmp(command, "unset") == 0)
        return UNSET;
    else if(strcmp(command, "showenv") == 0)
        return SHOWENV;
    else if(strcmp(command, "pushd") == 0)
        return PUSHD;
    else if(strcmp(command, "popd") == 0)
        return POPD;
    else if(strcmp(command, "dirs") == 0)
        return DIRS;
    else if(strcmp(command, "source") == 0)
        return SOURCE;
    else
        return EXTERNAL;
}

void echo(token_t *tokens, int start, int end)
{
    for(int i = start; i <= end; i++)
    {
        printf("%s ", tokens[i]);
    }

    printf("\n");
}


bool cd(dirnode **head, char *directory)
{
    // If chdir does not return 0, it means directory does not exist
    if(chdir(directory) == 0){
        char cwd[1000];
        getcwd(cwd, 1000);

        // Changing head of directory stack to mirror current directory
        strcpy((*head)->dir, cwd);
        
        return true;
    }

    return false;
}

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