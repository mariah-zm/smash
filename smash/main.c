#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise/linenoise.h"
#include "includes/parser.h"

int main(void) {
    
    // Get input and echo here
    char *line;

    while((line = linenoise("smash> ")) != NULL) {
        
        parse(line);
        
        // Freeing memory allocated by linenoise using linenoise
        linenoiseFree(line);
    }
    
    printf("Exiting smash...\n");
    return EXIT_SUCCESS;
}