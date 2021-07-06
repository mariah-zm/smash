#include "../internal.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "../redirection.h"
#include "../errors.h"

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
    if(!is_inredir()){
        for(int i = start; i <= end; i++)
            printf("%s ", tokens[i]);
        
        printf("\n");
    } else {
        char line[MAX_TOKEN_STRLEN];

        while(fgets(line, MAX_TOKEN_STRLEN, stdin)!= NULL)
            printf("%s", line);
    }
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

void showvar(shell_var *var_map, char *name)
{   
    if(name != NULL){
        char *value = get_shell_var(var_map, name);
        if(value != NULL)
            printf("%s=%s\n", name, value);
    } else {
        for(int i=0; i < MAX_VARIABLES; i++)
            if(var_map[i].name != NULL)
                printf("%s=%s\n", var_map[i].name, var_map[i].value);
    }
}

void showenv(shell_var *var_map, char *name)
{   
    if(name != NULL){
        printf("%s=%s\n", name, getenv(name));
    } else {
        for(int i=0; i < MAX_VARIABLES; i++)
            if(var_map[i].name != NULL && var_map[i].is_env)
                printf("%s=%s\n", var_map[i].name, var_map[i].value);
    }
}

int export(shell_var *var_map, char *name)
{
    int hash_index = get_hashcode(name);
    int i = hash_index;

    while(var_map[i].name != NULL){
        if(strcmp(var_map[i].name, name) == 0){        
            if(setenv(var_map[i].name, var_map[i].value, 1) != 0)
                return ERR_SET;
            else
                var_map[i].is_env = true;

            return OK;
        }

        i++;

        // Wrap around if the end of the hashtable is reached
        if(i == MAX_VARIABLES)
            i = 0;
        // This would mean that all elements were traversed and the variable was not found
        if(i == hash_index)
            return ERR_SET;
    }

    // This would mean that the variable with the given name was not found after the search
    return ERR_SET;
}

int unset(shell_var *var_map, char *name)
{
    int hash_index = get_hashcode(name);
    int i = hash_index;

    while(var_map[i].name != NULL){
        if(strcmp(var_map[i].name, name) == 0){
            free(var_map[i].name);
            free(var_map[i].value);

            var_map[i].name == NULL;
            var_map[i].value == NULL;

            if(var_map[i].is_env)
                unsetenv(name);

            return OK;
        }

        i = (i+1)%MAX_VARIABLES;
        
        // This would mean that all elements were traversed and the variable was not found
        if(i == hash_index)
            break;
    }

    // This would mean we have encountered an empty element after the given index but the var name was not found
    return ERR_UNSET;
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

void free_dirs(dirnode **head)
{
    dirnode *current = *head;
    dirnode *next;

    while(current != NULL){
        next = current->next_dir;
        free(current->dir);
        free(current);

        current = next;
    }
}