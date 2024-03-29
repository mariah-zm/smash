#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>

#include "../variables.h"
#include "../errors.h"
#include "../utils.h"

shell_var *init_shell_vars(char *shell, char **envp)
{
    shell_var *var_map = (shell_var *) malloc(sizeof(shell_var) * MAX_VARIABLES);

    if(var_map == NULL)
        return NULL;

    // Initialising all environment variables as shell variables
    while(*envp != NULL){
        if(var_assignment(var_map, *envp) != OK)
            return NULL;
        
        envp++;
    }

    // Setting PATH variable if not already set
    char *path;

    if((path = getenv("PATH")) == NULL){
        path = "/bin";
        insert_shell_var(var_map, "PATH", path, true);
    }

    // Setting USER and HOME variables
    char *user;
    char *home;

    int uid = geteuid();
    struct passwd *pass = getpwuid(uid);
    user = pass->pw_name;
    home = pass->pw_dir;

    insert_shell_var(var_map, "USER", user, true);
    insert_shell_var(var_map, "HOME", home, true);

    // Setting TERMINAL variable
    char *terminal_name = ttyname(STDOUT_FILENO);
    insert_shell_var(var_map, "TERMINAL", terminal_name, true);

    // Setting CWD variable
    char cwd[MAX_VAR_VALUE_STRLEN];
    getcwd(cwd, MAX_VAR_VALUE_STRLEN);
    insert_shell_var(var_map, "CWD", cwd, true);

    // Setting SHELL variable
    insert_shell_var(var_map, "SHELL", shell, true);

    // Setting PROMPT variable
    insert_shell_var(var_map, "PROMPT", PROMPT, true);

    // Setting EXITCODE variable
    insert_shell_var(var_map, "EXITCODE", "0", true);
         
    return var_map;    
}

int expand_var(shell_var *var_map, char* input, char* result)
{
    // Incrementing pointer to remove the $
    input++;

    char var_name[MAX_VAR_NAME_STRLEN];
    char* extra_chars = NULL;
    
    if(check_match(input, ENCLOSED_PATTERN) == MATCH){
        // Incrementing pointer to remove the {
        strcpy(var_name, ++input);

        int last_index = strlen(var_name);
        var_name[last_index-1] = '\0';
    } else if (check_match(input, ENCLOSED_MORE_CHARS_PATTERN) == MATCH){
        strcpy(var_name, ++input);
        extra_chars = strchr(input, '}');

        int index = strlen(var_name) - strlen(extra_chars);
        var_name[index] = '\0';

        // Incrementing to remove the '}'
        extra_chars++;
    } else if (check_match(input, NOT_ENCLOSED_PATTERN) == MATCH){
        int i = 0;
        while(is_var_name_char(*input)){
            var_name[i] = *input;
            i++;
            input++;
        }
        var_name[i] = '\0';
        extra_chars = input;
    } else {
        strcpy(result, "missing or extra, \'{\' \'}\'");

        return ERR_INVALID_SYNTAX;
    }

    if(is_var_name_valid(var_name)){
        char* value = get_shell_var(var_map, var_name);

        if(value != NULL)
            strcpy(result, value);

        if(extra_chars != NULL)
            strcat(result, extra_chars);
    } else {
        strcpy(result, "variable name is not valid");

        return ERR_INVALID_SYNTAX;
    }

    return OK;
}

void update_cwd(shell_var *var_map, char *dir)
{
    insert_shell_var(var_map, "CWD", dir, true);
    insert_shell_var(var_map, "PWD", dir, true);
}

bool is_var_name_char(char character)
{
    if(isalnum(character) || character == '_')
        return true;
    else 
        return false;
}

int var_assignment(shell_var *var_map, char *assignment)
{
    char *name = strtok(assignment, "=");
    char *value = strtok(NULL, "\0");

    if(!is_var_name_valid(name))
        return ERR_INVALID_SYNTAX;

    return insert_shell_var(var_map, name, value, false);
}

bool is_var_name_valid(char* name)
{
    if(strlen(name) <= MAX_VAR_NAME_STRLEN 
        && check_match(name, VAR_NAME_PATTERN) == MATCH)
        return true;
    else
        return false;
}

int insert_shell_var(shell_var *var_map, char *name, char* value, bool is_env)
{
    int hash_index = get_hashcode(name);
    int i = hash_index;

    while(strlen(var_map[i].name) != 0){
        // If a variable with that name already exists, update its contents
        if(strcmp(var_map[i].name, name) == 0)
            return update_shell_var(var_map, i, value);

        i = (i+1)%MAX_VARIABLES;

        // This would mean the hash table is full
        if(i == hash_index)
            return ERR_SET;
    }

    strcpy(var_map[i].name, name);
    strcpy(var_map[i].value, value);
    var_map[i].is_env = is_env;

    if(var_map[i].is_env) 
        if(setenv(var_map[i].name, var_map[i].value, 1) != 0)
            return ERR_SET;    

    return OK;
}

int update_shell_var(shell_var *var_map, int index, char *value)
{
    strcpy(var_map[index].value, value);

    if(var_map[index].is_env) 
        if(setenv(var_map[index].name, var_map[index].value, 1) != 0)
            return ERR_SET;   

    return OK;
}

char *get_shell_var(shell_var *var_map, char *name)
{
    int hash_index = get_hashcode(name);
    int i = hash_index;

    while(strlen(var_map[i].name) != 0){
        if(strcmp(var_map[i].name, name) == 0)
            return var_map[i].value;
        
        i = (i+1)%MAX_VARIABLES;
        
        // This would mean that we've traversed all elements and did not find the wanted var name
        if(i == hash_index)
            break;
    }

    return NULL;
}

int get_hashcode(char *name)
{  
    unsigned long hash = 17;
    for (int i = 0; i < strlen(name); i++) {
        hash = hash*31 + name[i];
    }

    return hash % MAX_VARIABLES;
}

void destroy_shell_vars(shell_var *var_map)
{
    free(var_map);
}