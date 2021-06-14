#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "parser.h"
#include "errors.h"
#include "matcher.h"
#include "variables.h"



void quote_removal(token_t tokens[MAX_NUM_TOKENS], int token_count)
{   
    for(int i=0; i < token_count; i++){
        remove_char(tokens[i], '\"');
        remove_char(tokens[i], '\\');
    }
}

bool is_assignment(char* string)
{
    if(strchr(string, '=') == NULL)
        return false;
    else 
        return true;
}

bool is_metachar_or_control_op(char *string)
{
    if(strcmp(string, ";") == 0
        || strcmp(string, "|") == 0
        || strcmp(string, "<") == 0
        || strcmp(string, ">") == 0
        || strcmp(string, "\n") == 0)
        return true;
    else
        return false;
}

void remove_char(char*string, char to_remove)
{
    char* ptr = strchr(string, to_remove);
    char removed[MAX_TOKEN_STRLEN];

    // Base case
    if(ptr == NULL)
        return;

    if(!is_escaped(ptr)){
        int index = strlen(string) - strlen(ptr);
        strcpy(removed, ++ptr);
        string[index] = '\0';
    }
    
    strcat(string, removed);

    // Recursive call
    remove_char(string, to_remove);
}