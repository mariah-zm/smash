#include <stdio.h>

#define OK 0
#define ERR_INIT 1
#define ERR_TOKEN_LENGTH 4
#define ERR_INVALID_SYNTAX 5
#define ERR_UNKNOWN_COMMAND 6

void print_error(int err_code, char* err_cause);

static void exit_shell()
{
    fprintf(stderr, "Failed to initialise shell variables");
}