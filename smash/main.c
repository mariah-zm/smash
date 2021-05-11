#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise/linenoise.h"
#include "includes/parser.h"
#include "includes/variables.h"
#include "includes/errors.h"

int main(void) {
    
    // Get input and echo here
    char *line;

    if (on_exit(exit_shell, "exiting"))
        perror("Cannot register exit handler");

    // Initialising shell variables on start up
    if(init_shell_vars() == ERR_INIT)
        exit(ERR_INIT);

    while((line = linenoise("smash> ")) != NULL) {

        parse(line);

        // Freeing memory allocated by linenoise using linenoise
        linenoiseFree(line);
    }
    
    return EXIT_SUCCESS;
}