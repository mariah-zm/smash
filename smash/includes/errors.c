#include <string.h>
#include <stdio.h>

#include "errors.h"

void print_error(int err_code, char* err_cause)
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
        default:
            strcpy(err_msg, "ERROR");
    }

    if(err_cause[0] != '\0')
        fprintf(stderr, "%s: %s\n", err_msg, err_cause);
    else
        fprintf(stderr, "%s\n", err_msg);
}

void exit_shell(int err_code, void* ptr)
{   
    switch(err_code)
    {
        case ERR_INIT:
            fprintf(stderr, "Failed to initialise shell variables.\n");
            break;
        case OK:
            printf("\n");
            break;
        default:
            printf("Error occured.\n");
    }

    printf("Exiting smash...\n");
}