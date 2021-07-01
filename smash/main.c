#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise/linenoise.h"
#include "includes/parser.h"
#include "includes/variables.h"
#include "includes/errors.h"
#include "includes/dirstack.h"


int main(int argc, char **argv, char **envp)
{
    // Get input and echo here
    char *line;
    shell_var *shell_variables;
    dirnode *head;

    if (on_exit(exit_shell, NULL))
        perror("Cannot register exit handler\n");

    // Initialising shell variables on start up
    if((shell_variables = init_shell_vars(argv[0], envp)) == NULL)
        exit(ERR_INIT);

    head = init_dir(get_shell_var(shell_variables, "HOME"));
    pushdir(&head, get_shell_var(shell_variables, "CWD"));

    // Since this is a pointer, when the contents of the shell variable change, the output will also be updated
    char *prompt = get_shell_var(shell_variables, "PROMPT");

    while((line = linenoise(prompt)) != NULL) {

        parse(line, head, shell_variables);

        // Freeing memory allocated by linenoise using linenoise
        linenoiseFree(line);
    }

    destroy_shell_vars(shell_variables);
    
    return OK;
}
