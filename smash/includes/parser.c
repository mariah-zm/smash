#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "errors.h"
#include "variables.h"

int tokenise(char* line, _token tokens[MAX_NUM_TOKENS])
{
    char* token = strtok(line, QUOTATION);

    // Check if user wants to exit
    if (token != NULL && strcmp(token, "exit") == 0)
        return 0;

    // Parse and copy tokens
    int i = 0;
    while(token != NULL && i < MAX_NUM_TOKENS - 1) {
        if(strlen(token) > MAX_TOKEN_STRLEN)
            return ERR_TOKEN_LENGTH;

        strcpy(tokens[i++], token);
        token = strtok(NULL, QUOTATION);
    }

    return 0;
}

void parse(char* line)
{
    int return_code;
    char result[MAX_TOKEN_STRLEN];

    _token tokens[MAX_NUM_TOKENS];

    if(line[0] == DOLLAR_SIGN){
        if((return_code = expand_var(line, result)) != 0)
            print_error(return_code, result);
        else 
            printf("%s\n", result);
    } else 
        if((return_code = tokenise(line, tokens)) != 0) {
            char error[50];

            snprintf(error, 50, "maximum token length is %d", MAX_TOKEN_STRLEN);

            print_error(return_code, error);
        }
}