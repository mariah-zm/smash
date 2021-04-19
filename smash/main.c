#include <stdio.h>
#include <stdlib.h>
#include "linenoise/linenoise.h"

int main(void) {
    
    // Get input and echo here
    char *line;
    
    while((line = linenoise("smash> ")) != NULL) {
        printf("%s\n", line);

        // Freeing memory allocated by linenoise using linenoise
        linenoiseFree(line);
    }
    
    printf("Exiting\n");
    return EXIT_SUCCESS;
}