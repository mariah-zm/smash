#include <regex.h>
#include "tokeniser.h"

int tokenise(char* input)
{
    token_t tokens[MAX_NUM_TOKENS];

    char* token = strtok(input, SPACE);

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