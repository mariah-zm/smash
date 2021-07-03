#include "../matcher.h"

#include <regex.h>
#include <stddef.h>
#include <string.h>

#include "../tokeniser.h"

int check_match(char *input, char *pattern)
{
    // Validating variable name
    regex_t regex;
    int match;

    if(regcomp(&regex, pattern, REG_EXTENDED) == 0)
        match = regexec(&regex, input, 0, NULL, 0);
    else 
        return -1;

    regfree(&regex);

    if(match == 0)
        return MATCH;
    else
        return NOMATCH;
}

void quote_removal(char **strings, int count)
{   
    for(int i=0; i < count; i++){
        remove_char(strings[i], '\"');
        remove_char(strings[i], '\\');
    }
}

bool is_assignment(char *string)
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

void remove_char(char *string, char to_remove)
{
    char *ptr = strchr(string, to_remove);
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
