#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise/linenoise.h"
#include "includes/tokenise.h"

int main(void) {
    
    // Get input and echo here
    char *line;

    char *metacharacters = " ";

    while((line = linenoise("smash> ")) != NULL) {
        _token token = strtok(line, metacharacters);

        // Check if user wants to exit
        if (token != NULL && strcmp(token, "exit") == 0)
            break;

        _token tokens[MAX_TOKEN_SIZE];

        // Parse and copy tokens
        int i = 0;
        while(token != NULL && i < MAX_TOKEN_SIZE - 1) {
            printf("%s\n", token);
            tokens[i++] = token;
            token = strtok(NULL, metacharacters);
        }

        // Freeing memory allocated by linenoise using linenoise
        linenoiseFree(line);
    }
    
    printf("Exiting smash...\n");
    return EXIT_SUCCESS;
}