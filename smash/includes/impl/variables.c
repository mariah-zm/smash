#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h>

#include "../variables.h"
#include "../errors.h"
#include "../matcher.h"

shell_var *init_shell_vars(char *shell, char **envp)
{
    shell_var *var_map = (shell_var *) malloc(sizeof(shell_var) * MAX_VARIABLES);

    if(var_map == NULL)
        return NULL;

    // Initialising all environment variables as shell variables
    char name[MAX_VAR_NAME_STRLEN];
    char value[MAX_VAR_VALUE_STRLEN];

    while(*envp != NULL){
        if(var_assignment(*envp, name, value) != 0)
            return NULL;
        
        insert_shell_var(var_map, name, value, true);
        
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

int var_assignment(char *assignment, char *var_name, char *var_value)
{
    char *name = strtok(assignment, "=");
    char *value = strtok(NULL, "\0");

    if(!is_var_name_valid(name))
        return ERR_INVALID_SYNTAX;

    strcpy(var_name, name);
    strcpy(var_value, value);
     
    return OK;
}

bool is_var_name_valid(char* name)
{
    if(strlen(name) <= MAX_VAR_NAME_STRLEN 
        && check_match(name, VAR_NAME_PATTERN) == MATCH)
        return true;
    else
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

int export_shell_var(shell_var *var_map, char *name)
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

void destroy_shell_vars(shell_var *var_map)
{
    free(var_map);
}

int update_shell_var(shell_var *var_map, int index, char *value)
{
    var_map[index].value = (char *) realloc(var_map[index].value, strlen(value) + 1);
    strcpy(var_map[index].value, value);

    if(var_map[index].is_env) 
        if(setenv(var_map[index].name, var_map[index].value, 1) != 0)
            return ERR_SET;   

    return OK;
}

int insert_shell_var(shell_var *var_map, char *name, char* value, bool is_env)
{
    int hash_index = get_hashcode(name);
    int i = hash_index;

    while(var_map[i].name != NULL){
        // If a variable with that name already exists, update its contents
        if(strcmp(var_map[i].name, name) == 0)
            return update_shell_var(var_map, i, value);

        i = (i+1)%MAX_VARIABLES;

        // This would mean the hash table is full
        if(i == hash_index)
            return ERR_SET;
    }

    var_map[i].name = (char *) malloc(strlen(name) + 1);
    var_map[i].value = (char *) malloc(strlen(value) + 1);

    // Return if memory wasn't allocated properly
    if(var_map[i].name == NULL || var_map[i].value == NULL)
        return ERR_SET;

    strcpy(var_map[i].name, name);
    strcpy(var_map[i].value, value);
    var_map[i].is_env = is_env;

    if(var_map[i].is_env) 
        if(setenv(var_map[i].name, var_map[i].value, 1) != 0)
            return ERR_SET;    

    return OK;
}

int remove_shell_var(shell_var *var_map, char *name)
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

char *get_shell_var(shell_var *var_map, char *name)
{
    int hash_index = get_hashcode(name);
    int i = hash_index;

    while(var_map[i].name != NULL){
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