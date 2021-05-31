#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise/linenoise.h"
#include "includes/parser.h"
#include "includes/variables.h"
#include "includes/errors.h"

int main(int argc, char **argv, char **envp)
{
    // Get input and echo here
    char *line;

    if (on_exit(exit_shell, NULL))
        perror("Cannot register exit handler\n");

    // Initialising shell variables on start up
    if(init_shell_vars(argv[0], envp) == ERR_INIT)
        exit(ERR_INIT);

    // Since this is a pointer, when the contents of the shell variable change, the output will also be updated
    char *prompt = get_shell_var("PROMPT");

    while((line = linenoise(prompt)) != NULL) {

        parse(line);

        // Freeing memory allocated by linenoise using linenoise
        linenoiseFree(line);
    }

    // to put in exit function
    destroy_shell_vars();
    
    return OK;
}