#include "executer.h"

#include "errors.h"
#include "variables.h"
#include "dirstack.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "stdlib.h"

void execute(token_t *tokens, int start, int end, dirnode *head, shell_var *shell_variables)
{
    int command = which_command(tokens[start]);
    // incrementing start pointer to skip command
    start++;

    switch(command){
        case EXIT:
            exit(OK);
            break;
        case ECHO:
            echo(tokens, start, end);
            break;
        case CD:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                if(cd(&head, tokens[1])){
                    update_cwd(shell_variables, tokens[1]);
                } else {
                    char err_msg[200] = "could not locate directory ";
                    strcat(err_msg, tokens[1]);
                    print_error(ERR_DIR, err_msg);
                }
            break;
        case SHOWVAR:
            if(end == 0)
                showvar(shell_variables, NULL);
            else if(end == start)
                showvar(shell_variables, tokens[1]);
            else 
                print_error(ERR_ARGS, "expected less than 1 arguments");
            break;
        case EXPORT:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                export_shell_var(shell_variables, tokens[1]);
            break;
        case UNSET:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                remove_shell_var(shell_variables, tokens[1]);
            break;
        case SHOWENV:
            if(end == 0)
                showenv(shell_variables, NULL);
            else if(end == start)
                showenv(shell_variables, tokens[1]);
            else 
                print_error(ERR_ARGS, "expected less than 1 arguments");
            break;
        case PUSHD:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else{
                if(pushdir(&head, tokens[1])){
                    update_cwd(shell_variables, head->dir);
                    dirs(head);
                } else {
                    char err_msg[200] = "could not locate directory ";
                    strcat(err_msg, tokens[1]);
                    print_error(ERR_DIR, err_msg);
                }
            }
            break;
        case POPD:
            if(end > 1)
                print_error(ERR_ARGS, "expected 0 arguments");
            else{
                popdir(&head);
                update_cwd(shell_variables, head->dir);
                dirs(head);
            }
            break;
        case DIRS:
            if(end > 1)
                print_error(ERR_ARGS, "expected 0 arguments");
            else
                dirs(head);
            break;
        case SOURCE:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                execute_source(tokens[1], head, shell_variables);
            break;
        case EXTERNAL:
            printf("Command not found\n");
            break;
    }
}

//file name either relative or absolute
void execute_source(char *file_name, dirnode *head, shell_var *shell_variables)
{
    FILE *fp;

    // If file is not found, false is returned
    if((fp = fopen(file_name, "r")) == NULL)
    {
        char err_msg[200] = "could not locate the file ";
        strcat(err_msg, file_name);
        print_error(ERR_FILE_NOT_FOUND, err_msg);
    } else {
        token_t line;

        while(fgets(line, MAX_TOKEN_STRLEN, fp) != NULL)
            parse(line, head, shell_variables);

        if (fclose(fp) != 0)
            print_error(ERR_FILE_NOT_FOUND, "error closing file");

    }
}

bool cd(dirnode **head, char *directory)
{
    if(chdir(directory) == 0){
        char cwd[1000];
        getcwd(cwd, 1000);

        strcpy((*head)->dir, cwd);
        
        return true;
    }

    return false;
}

void echo(token_t *tokens, int start, int end)
{
    for(int i = start; i <= end; i++){
        printf("%s ", tokens[i]);
    }

    printf("\n");
}

void external(token_t *tokens, int start, int end)
{

}

char **env()
{

}

int which_command(token_t command)
{
    if(strcmp(command, "exit") == 0)
        return EXIT;
    else if(strcmp(command, "echo") == 0)
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