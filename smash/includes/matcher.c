#include "matcher.h"

#include <regex.h>
#include <stddef.h>
#include <string.h>

int check_match(char* input, char* pattern)
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

