#include "../errors.h"

#include <string.h>
#include <stdio.h>

void print_error(int err_code, char *err_cause)
{
    char err_msg[100];

    switch(err_code)
    {
        case ERR_SET:
            strcpy(err_msg, "COULD NOT SET SHELL VARIABLE");
            break;
        case ERR_UNSET:
            strcpy(err_msg, "COULD NOT UNSET SHELL VARIABLE");
            break;
        case ERR_TOKEN_LENGTH:
            strcpy(err_msg, "INVALID TOKEN LENGTH");
            break; 
        case ERR_INVALID_SYNTAX:
            strcpy(err_msg, "INVALID SYNTAX");
            break;
        case ERR_UNKNOWN_COMMAND:
            strcpy(err_msg, "UNKNOWN COMMAND");
            break;
        case ERR_ARGS:
            strcpy(err_msg, "ARGUMENTS ERROR");
            break;
        case ERR_DIR:
            strcpy(err_msg, "DIRECTORY ERROR");
            break;
        case ERR_FILE_NOT_FOUND:
        case ERR_FILE_EMPTY:
            strcpy(err_msg, "FILE ERROR");
            break;
        case ERR_PIPE:
            strcpy(err_msg, "PIPELINE ERROR");
            break;
        default:
            strcpy(err_msg, "ERROR");
    }

    if(err_cause != NULL)
        fprintf(stderr, "%s: %s\n", err_msg, err_cause);
    else
        fprintf(stderr, "%s\n", err_msg);
}

void exit_shell(int err_code, void *ptr)
{   
    switch(err_code)
    {
        case ERR_INIT:
            fprintf(stderr, "INITIALISATION ERROR: Failed to initialise shell variables.\n");
            break;
        case OK:
            printf("\n");
            break;
        default:
            printf("Error occured.\n");
    }

    printf("Exiting smash...\n");
}