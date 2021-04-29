#include <stdio.h>
#include <string.h>

#include "errors.h"

void print_error(int err_code, char* err_cause)
{
    char err_msg[30];

    switch(err_code)
    {
        case ERR_TOKEN_LENGTH:
            strcpy(err_msg, "INVALID TOKEN LENGTH");
            break; 
        case ERR_INVALID_SYNTAX:
            strcpy(err_msg, "INVALID SYNTAX");
            break;
        default:
            strcpy(err_msg, "ERROR");
    }

    fprintf(stderr, "%s: %s\n", err_msg, err_cause);
}