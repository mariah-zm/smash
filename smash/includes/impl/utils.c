#include "../utils.h"

#include <regex.h>
#include <stddef.h>
#include <string.h>

#include "../tokeniser.h"
#include "../errors.h"

int check_match(char *input, char *pattern)
{
    regex_t regex;
    int match;

    if(regcomp(&regex, pattern, REG_EXTENDED) == 0)
        match = regexec(&regex, input, 0, NULL, 0);
    else 
        return ERR_GENERIC;

    regfree(&regex);

    if(match == 0)
        return MATCH;
    else
        return NOMATCH;
}

void quote_removal(token_t *tokens, int token_count)
{   
    for(int i=0; i < token_count; i++){
        remove_char(tokens[i], '\"');
        remove_char(tokens[i], '\\');
    }
}

void remove_char(token_t token, char to_remove)
{
    char *ptr = strchr(token, to_remove);
    char removed[MAX_TOKEN_STRLEN];

    // Base case
    if(ptr == NULL)
        return;

    if(!is_escaped(ptr)){
        int index = strlen(token) - strlen(ptr);
        strcpy(removed, ++ptr);
        token[index] = '\0';

        strcat(token, removed);

        // Recursive call
        remove_char(token, to_remove);
    } else{
        // Skip escaped character and leave it in token and search for quotes from that point onwards
        int index = strlen(token) - strlen(++ptr);
        // Must save contexnts of ptr first since it is a pointer!
        strcpy(removed, ptr);
        token[index] = '\0';

        remove_char(removed, to_remove);

        // After returning, all characters would have been removed
        strcat(token, removed);
    }
}

bool is_quoted(char* full_string, char* sub_string)
{
    if(sub_string == NULL)
        return false;

    // If only one quote exists before the sub-string, it means the sub-string is within a quoted string
    return check_match(sub_string, IS_CHAR_QUOTED_PATTERN);
}

bool is_escaped(char* ptr)
{
    if(ptr == NULL)
        return false;

    // Decrementing pointer to check whether the character before the start of pointer is a backslash
    ptr--;

    if(*ptr == '\\' && *(--ptr) != '\\')
        return true;
    else 
        return false;
}


bool is_assignment(token_t token)
{
    if(strchr(token, '=') == NULL)
        return false;
    else 
        return true;
}

bool is_metachar(token_t token)
{
    if(strcmp(token, ";") == 0
        || strcmp(token, "|") == 0
        || strcmp(token, "<") == 0
        || strcmp(token, ">") == 0)
        return true;
    else
        return false;
}

int get_pos(token_t *tokens, int token_count, char *symbol)
{
    for(int i=0; i < token_count; i++){
        if(strcmp(tokens[i], symbol) == 0){
            return i;
        }
    }

    return -1;
}