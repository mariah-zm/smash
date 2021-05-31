#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <sys/types.h>
#include <pwd.h>

#include "variables.h"
#include "errors.h"
#include "matcher.h"

shell_var *shell_variables;

int init_shell_vars(char *shell, char **envp)
{
    shell_variables = (shell_var *) malloc(sizeof(shell_var) * MAX_VARIABLES);

    if(shell_variables == NULL)
        return ERR_INIT;

    // Initialising all environment variables as shell variables
    char name[MAX_VAR_NAME_STRLEN];
    char value[MAX_VAR_VALUE_STRLEN];

    while(*envp != NULL){
        if(var_assignment(*envp, name, value) != 0)
            return ERR_INIT;
        
        insert_shell_var(name, value, true);
        
        envp++;
    }

    // Setting PATH variable if not already set
    char *path;

    if((path = getenv("PATH")) == NULL){
        path = "/bin";
        insert_shell_var("PATH", path, true);
    }

    // Setting USER and HOME variables
    char *user;
    char *home;

    int uid = geteuid();
    struct passwd *pass = getpwuid(uid);
    user = pass->pw_name;
    home = pass->pw_dir;

    insert_shell_var( "USER", user, true);
    insert_shell_var("HOME", home, true);

    // Setting TERMINAL variable
    char *terminal_name = ttyname(STDOUT_FILENO);
    insert_shell_var("TERMINAL", terminal_name, true);

    // Setting CWD variable
    char cwd[MAX_VAR_VALUE_STRLEN];
    getcwd(cwd, MAX_VAR_VALUE_STRLEN);
    insert_shell_var("CWD", cwd, true);

    // Setting SHELL variable
    insert_shell_var("SHELL", shell, true);

    // Setting PROMPT variable
    insert_shell_var("PROMPT", PROMPT, true);

    // Setting EXITCODE variable
    insert_shell_var("EXITCODE", "0", true);
         
    return OK;    
}

int expand_var(char* input, char* result)
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
    } else if (check_match(input, NOT_ENCLOSED_PATTERN) == MATCH){
        strcpy(var_name, input);
    } else {
        strcpy(result, "missing or extra, \'{\' \'}\'");

        return ERR_INVALID_SYNTAX;
    }

    if(is_var_name_valid(var_name)){
        char* value = get_shell_var(var_name);

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

void showvar(char *name)
{   
    if(name != NULL){
        char *value = get_shell_var(name);
        if(value != NULL)
            printf("%s=%s\n", name, value);
    } else {
        for(int i=0; i < MAX_VARIABLES; i++)
            if(shell_variables[i].name != NULL)
                printf("%s=%s\n", shell_variables[i].name, shell_variables[i].value);
    }
}

int export_shell_var(char *name)
{
    return update_shell_var(-1, name, NULL, true);
}

void destroy_shell_vars()
{
    free(shell_variables);
}

int update_shell_var(int index, char *name, char *value, bool is_env)
{
    if(index == -1){
        int hash_index = get_hashcode(name);
        int i = hash_index;

        while(shell_variables[i].name != NULL){
            // If the correct index is found break from loop
            if(strcmp(shell_variables[i].name, name) == 0){
                index = i;
                break;
            }

            i++;

            // Wrap around if the end of the hashtable is reached
            if(i == MAX_VARIABLES)
                i = 0;
            // This would mean that all elements were traversed and the variable was not found
            if(i == hash_index)
                return -1;
        }
    }

    // This would mean that the variable with the given name was not found after the search
    if(index == -1)
        return ERR_SET;

    // If value is NULL, it means the calling function was export   
    if(value != NULL){
        shell_variables[index].value = (char *) realloc(shell_variables[index].value, strlen(value) + 1);
        strcpy(shell_variables[index].value, value);
    }
    shell_variables[index].is_env = is_env;

    if(shell_variables[index].is_env) 
        if(setenv(shell_variables[index].name, shell_variables[index].value, 1) != 0)
            return ERR_SET;    

    return OK;
}

int insert_shell_var(char *name, char* value, bool is_env)
{
    int hash_index = get_hashcode(name);
    int i = hash_index;

    while(shell_variables[i].name != NULL){
        // If a variable with that name already exists, update its contents
        if(strcmp(shell_variables[i].name, name) == 0)
            return update_shell_var(i, name, value, shell_variables[i].is_env);

        i++;

        // Wrap around if the end of the hashtable is reached
        if(i == MAX_VARIABLES)
            i = 0;
        // This would mean the hash table is full
        if(i == hash_index)
            return ERR_SET;
    }

    shell_variables[i].name = (char *) malloc(strlen(name) + 1);
    shell_variables[i].value = (char *) malloc(strlen(value) + 1);

    // Return if memory wasn't allocated properly
    if(shell_variables[i].name == NULL || shell_variables[i].value == NULL)
        return ERR_SET;

    strcpy(shell_variables[i].name, name);
    strcpy(shell_variables[i].value, value);
    shell_variables[i].is_env = is_env;

    if(shell_variables[i].is_env) 
        if(setenv(shell_variables[i].name, shell_variables[i].value, 1) != 0)
            return ERR_SET;    

    return OK;
}

int remove_shell_var(char *name)
{
    int hash_index = get_hashcode(name);
    int i = hash_index;

    while(shell_variables[i].name != NULL){
        if(strcmp(shell_variables[i].name, name) == 0){
            free(shell_variables[i].name);
            free(shell_variables[i].value);

            shell_variables[i].name == NULL;
            shell_variables[i].value == NULL;

            if(shell_variables[i].is_env)
                unsetenv(name);

            return OK;
        }

        // Wrap around
        if(i+1 == MAX_VARIABLES)
            i=0;
        else 
            i++;
        
        // This would mean that all elements were traversed and the variable was not found
        if(i == hash_index)
            break;
    }

    // This would mean we have encountered an empty element after the given index but the var name was not found
    return -1;
}

char *get_shell_var(char *name)
{
    int hash_index = get_hashcode(name);
    int i = hash_index;

    while(shell_variables[i].name != NULL){
        if(strcmp(shell_variables[i].name, name) == 0)
            return shell_variables[i].value;
        
        i++;

        // Wrap around if at the end of hashtable
        if(i == MAX_VARIABLES)
            i=0;
        
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