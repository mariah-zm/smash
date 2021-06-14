#include "executer.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

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