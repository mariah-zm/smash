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
    char result[MAX_ENV_VALUE_STRLEN];

    if (on_exit(exit_shell, NULL))
        perror("Cannot register exit handler\n");

    // Initialising shell variables on start up
    if(init_shell_vars() == ERR_INIT)
        exit(ERR_INIT);

    while((line = linenoise(PROMPT)) != NULL) {

        // parse(line);
        expand_var(line, result);
        printf("%s\n", result);

        // Freeing memory allocated by linenoise using linenoise
        linenoiseFree(line);
    }
    
    return OK;
}