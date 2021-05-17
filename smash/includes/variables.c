#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>

#include "variables.h"
#include "errors.h"
#include "utils.h"

int init_shell_vars()
{
    char* path = getenv("PATH");
    char* user = getenv("USER");
    char* home = getenv("HOME");
    char* terminal_name = ttyname(STDOUT_FILENO);

    char cwd[MAX_VAR_VALUE_STRLEN] = "\0";
    char shell[MAX_VAR_VALUE_STRLEN] = "\0";
    getcwd(cwd, MAX_VAR_VALUE_STRLEN);
    getcwd(shell, MAX_VAR_VALUE_STRLEN);

    if(path == NULL 
        || cwd[0] == '\0'
        || user == NULL 
        || home == NULL
        || shell[0] == '\0'
        || terminal_name == NULL)
        return ERR_INIT;

    strcat(shell, "/bin/smash");

    if(setenv("PATH", path, 1) == -1
        || setenv("PROMPT", PROMPT, 1) == -1
        || setenv("CWD", cwd, 1) == -1
        || setenv("USER", user, 1) == -1
        || setenv("HOME", home, 1) == -1
        || setenv("SHELL", shell, 1) == -1
        || setenv("TERMINAL", terminal_name, 1) == -1
        || setenv("EXITCODE", "0", 1) == -1)
        return ERR_INIT;

    return OK;    
}

int expand_var(char* input, char* result)
{
    // Incrementing pointer to remove the $
    input++;

    char var_name[MAX_VAR_NAME_STRLEN];
    char* extra_chars = NULL;
    
    if(is_matched(input, ENCLOSED_PATTERN)){
        // Incrementing pointer to remove the {
        strcpy(var_name, ++input);

        int last_index = strlen(var_name);
        var_name[last_index-1] = '\0';
    } else if (is_matched(input, ENCLOSED_MORE_CHARS_PATTERN)){
        strcpy(var_name, ++input);
        extra_chars = strchr(input, '}');

        int index = strlen(var_name) - strlen(extra_chars);
        var_name[index] = '\0';
    } else if (is_matched(input, NOT_ENCLOSED_PATTERN)){
        strcpy(var_name, input);
    } else {
        strcpy(result, "missing or extra, \'{\' \'}\'");

        return ERR_INVALID_SYNTAX;
    }

    if(is_var_name_valid(var_name)){
        char* value = getenv(var_name);

        if(value != NULL)
            strcpy(result, value);

        if(extra_chars != NULL)
            strcat(result, ++extra_chars);
    } else {
        strcpy(result, "variable name is not valid");

        return ERR_INVALID_SYNTAX;
    }

    return OK;
}

int set_shell_var(char* assignment)
{
    // Chcecking if the given assignment is in the correct syntax
    if(!is_matched(assignment, ASSIGNMENT_PATTERN))
        return ERR_INVALID_SYNTAX;

    char* name = strtok(assignment, "=");
    assignment++;
    char* value = strtok(NULL, "\0");

    if(!is_var_name_valid(name))
        return ERR_INVALID_SYNTAX;

    if(setenv(name, value, 1) == -1)
        return ERR_SET;

    return OK;
}

bool is_var_name_valid(char* name)
{
    if(strlen(name) <= MAX_VAR_NAME_STRLEN 
        && is_matched(name, VAR_NAME_PATTERN))
        return true;
    else
        return false;
}
