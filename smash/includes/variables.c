#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>

#include "variables.h"
#include "errors.h"
#include "parser.h"

int num_vars = 0;

int init_shell_vars()
{
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

    int return_code = OK;

    char var_name[VAR_NAME_STRLEN];
    char* extra_chars = NULL;

    regex_t regex_enclosed;
    regex_t regex_enclosed_more_chars;
    regex_t regex_not_enclosed;

    char* pattern_enclosed = "^[{][^{}]+[}]$";
    char* pattern_enclosed_more_chars = "^[{][^{}]+[}].+";
    char* pattern_not_enclosed = "^[^{]+";

    regcomp(&regex_enclosed, pattern_enclosed, REG_EXTENDED);
    regcomp(&regex_enclosed_more_chars, pattern_enclosed_more_chars, REG_EXTENDED);
    regcomp(&regex_not_enclosed, pattern_not_enclosed, 0);
    
    if(regexec(&regex_enclosed, input, 0, NULL, 0) == 0){
        // Incrementing pointer to remove the {
        strcpy(var_name, ++input);

        int last_index = strlen(var_name);
        var_name[last_index-1] = '\0';
    } else if (regexec(&regex_enclosed_more_chars, input, 0, NULL, 0) == 0){
        strcpy(var_name, ++input);
        extra_chars = strchr(input, '}');

        int index = strlen(var_name) - strlen(extra_chars);
        var_name[index] = '\0';
    } else if (regexec(&regex_not_enclosed, input, 0, NULL, 0) == 0){
        strcpy(var_name, input);
    } else {
        strcpy(result, "missing or extra, \'{\' \'}\'");

        return_code = ERR_INVALID_SYNTAX;
    }

    if(return_code != ERR_INVALID_SYNTAX){
        if(is_var_name_valid(var_name) != ERR_INVALID_SYNTAX){
            char* value = getenv(var_name);

            if(value != NULL)
                strcpy(result, value);

            if(extra_chars != NULL)
                strcat(result, ++extra_chars);
        } else {
            strcpy(result, "variable name is not valid");

            return_code = ERR_INVALID_SYNTAX;
        }
    }

    regfree(&regex_enclosed);
    regfree(&regex_enclosed_more_chars);
    regfree(&regex_not_enclosed);

    return return_code;
}

int set_shell_var(char* name, char* value)
{
    if(is_var_name_valid(name) == ERR_INVALID_SYNTAX)
        return ERR_INVALID_SYNTAX;

    if(setenv(name, value, 1) == -1)
        return ERR_SET;

    return OK;
}

int is_var_name_valid(char* name)
{
    // Validating variable name
    regex_t regex;
    int match;
    char* regex_pattern = "^[a-zA-Z_][a-zA-Z0-9_]*$";

    regcomp(&regex, regex_pattern, REG_EXTENDED);
    match = regexec(&regex, name, 0, NULL, 0);

    regfree(&regex);

    if(match != 0)
        return ERR_INVALID_SYNTAX;
    else
        return OK;
}
