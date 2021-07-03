#include "../external.h"

#define _GNU_SOURCE

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <wait.h>

#include "../errors.h"
#include "../variables.h"

int external(token_t *tokens, int token_count, shell_var *shell_variables)
{
    int env_count;
    char **env = build_env(shell_variables, &env_count);
    char PATH[MAX_VAR_VALUE_STRLEN];
    strcpy(PATH, get_shell_var(shell_variables, "PATH"));

    char *subpath = strtok(PATH, ":");
    char *subpath_arr[MAX_NUM_TOKENS];

    int i = 0;
    while(subpath != NULL && i < MAX_VARIABLES - 1) 
    {
            subpath_arr[i++] = subpath;
            subpath = strtok(NULL, ":");
    }
    subpath_arr[i] = NULL;
    int num_subpaths = i;

    // Searching for command binary
    i = 0;
    char command_binary[MAX_TOKEN_STRLEN];
    while(i < num_subpaths)
    {
        strcpy(command_binary, subpath_arr[i]);
        strcat(command_binary, "/");
        strcat(command_binary, tokens[0]);

        if(access(command_binary, F_OK) == 0)
            break;
        
        i++;
    }

    if(i >= num_subpaths)
        return ERR_UNKNOWN_COMMAND;

    // Creating a new array of arguments, terminated by NULL, to be passed to fork_plus_exec
    char *args[MAX_NUM_TOKENS];
    int j;

    for(j=0; j < token_count; j++)
    {
        args[j] = tokens[j];
    }
    args[j] = NULL;

    fork_plus_exec(args[0], args, env);

    destroy_env(env, env_count);

    return OK;
}

void fork_plus_exec(char *command, char **args, char **env)
{
    pid_t pid = fork();

    if(pid == 0){
        if (execve(args[0], args, env))
        {
            print_error(ERR_GENERIC, "execve failed");
            // calling _Exit to avoid exithandler
            _Exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        waitpid(pid, NULL, WUNTRACED);
    } else{
        print_error(ERR_GENERIC, "could not start fork");
    }
}

char **build_env(shell_var *shell_variables, int *count)
{
    char **env = (char**) malloc(MAX_VARIABLES * sizeof(char*));

    int env_i = 0;

    for(int i=0; i < MAX_VARIABLES; i++)
    {
        if(shell_variables[i].name != NULL)
        {
            int str_len = strlen(shell_variables[i].name) + strlen(shell_variables[i].value) + 2;
            env[env_i] = (char *) malloc(str_len);

            strcpy(env[env_i], shell_variables[i].name);
            strcat(env[env_i], "=");
            strcat(env[env_i], shell_variables[i].value);

            env_i++;
        }
    }

    env[env_i] = NULL;

    *count = env_i;

    return env;
}

void destroy_env(char **env, int count)
{
    for(int i=0; i < count; i++)
    {
        free(env[i]);
    }
    free(env);
}

