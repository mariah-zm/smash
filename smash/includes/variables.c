#include <string.h>

#include "tokenise.h"
#include "variables.h"
#include "errors.h"

int expand_var(char* input, char* result)
{
    // Incrementing pointer to remove the $
    input++;

    int last_index;
    char var_name[MAX_TOKEN_STRLEN];

    if(input[0] == '{'){
        strcpy(var_name, ++input);
        
        last_index = strlen(var_name);

        if(var_name[last_index-1] != '}'){
            strcpy(result, "missing \'}\'");
            return ERR_INVALID_SYNTAX;
        } else
            var_name[last_index-1] = '\0';

    } else {
        strcpy(var_name, input);

        last_index = strlen(var_name);

        if(var_name[last_index-1] == '}'){
            strcpy(result, "missing \'{\'");
            return ERR_INVALID_SYNTAX;
        }
    }

    strcpy(result, var_name);

    return 0;
}