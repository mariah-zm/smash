#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "variables.h"
#include "errors.h"
#include "parser.h"

char shell_variables[100][VAR_NAME_STRLEN];

int num_vars = 0;

int init_shell_vars()
{
    typedef enum {PATH, PROMPT, CWD, USER, HOME, SHELL, TERMINAL, EXITCODE} shell_var;

    char* path = getenv("PATH");
    char* user = getenv("USER");
    char* home = getenv("HOME");
    char* terminal_name = ttyname(STDOUT_FILENO);

    char cwd[MAX_TOKEN_STRLEN] = "\0";
    char shell[MAX_TOKEN_STRLEN] = "\0";
    getcwd(cwd, MAX_TOKEN_STRLEN);
    getcwd(shell, MAX_TOKEN_STRLEN);

    strcat(shell, "/bin/smash");
 
    if(path == NULL 
        || cwd[0] == '\0'
        || user == NULL 
        || home == NULL
        || shell[0] == '\0'
        || terminal_name == NULL)
        return ERR_INIT;

    if(setenv("PATH", path, 1) == -1
        || setenv("PROMPT", "smash> ", 1) == -1
        || setenv("CWD", cwd, 1) == -1
        || setenv("USER", user, 1) == -1
        || setenv("HOME", home, 1) == -1
        || setenv("SHELL", shell, 1) == -1
        || setenv("TERMINAL", terminal_name, 1) == -1
        || setenv("EXITCODE", "0", 1) == -1)
        return ERR_INIT;

    add_var("PATH");
    add_var("PROMPT");
    add_var("CWD");
    add_var("USER");
    add_var("HOME");
    add_var("SHELL");
    add_var("TERMINAL");
    add_var("EXITCODE");

    return OK;    
}

int expand_var(char* input, char* result)
{
    // Incrementing pointer to remove the $
    input++;

    int last_index;
    char var_name[VAR_NAME_STRLEN];

    if(input[0] == '{'){
        strcpy(var_name, ++input);
        
        last_index = strlen(var_name);

        if(var_name[last_index-1] != '}'){
            strcpy(result, "missing \'}\'");
            return ERR_INVALID_SYNTAX;
        } else
            var_name[last_index-1] = '\0';

    } else {
        strcpy(var_name, input);

        last_index = strlen(var_name);

        if(var_name[last_index-1] == '}'){
            strcpy(result, "missing \'{\'");
            return ERR_INVALID_SYNTAX;
        }
    }

    strcpy(result, getenv(var_name));

    return 0;
}

int set_env_var(char* statement)
{

}

void add_var(char* name)
{
    strcpy(shell_variables[num_vars++], name);
}