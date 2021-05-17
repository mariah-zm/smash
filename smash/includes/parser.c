#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "parser.h"
#include "errors.h"
#include "utils.h"
#include "tokeniser.h"

void parse(char* line)
{
    int return_code;
    token_t tokens[MAX_NUM_TOKENS];

    if(strchr(line, '\"') != NULL && !is_matched(line, QUOTATION_PATTERN))
        print_error(ERR_INVALID_SYNTAX, "missing \"");
    else {
        int token_count = 0;
        int* count_ptr = &token_count;

        tokenise(tokens, line, count_ptr, 0);

        for(int i=0; i < token_count; i++){
            quote_removal(tokens[i], '\"');
            quote_removal(tokens[i], '\\');
            printf("%s\n", tokens[i]);
        }
    }
}

void quote_removal(char* string, char to_remove)
{   
    char* ptr = strchr(string, to_remove);

    // Base case
    if(ptr == NULL)
        return;

    if(!is_escaped(ptr)){
        int index = strlen(string) - strlen(ptr);
        string[index] = '\0';
    }

    strcat(string, ++ptr);

    // Recursive call
    quote_removal(string, to_remove);
}