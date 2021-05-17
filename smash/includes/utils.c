#include "utils.h"
#include "tokeniser.h"

#include <regex.h>
#include <stddef.h>
#include <string.h>

bool is_matched(char* input, char* pattern)
{
    // Validating variable name
    regex_t regex;
    int match;

    regcomp(&regex, pattern, REG_EXTENDED);
    match = regexec(&regex, input, 0, NULL, 0);

    regfree(&regex);

    if(match != 0)
        return false;
    else
        return true;
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