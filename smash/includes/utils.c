#include "utils.h"

#include <regex.h>
#include <stddef.h>

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