#include <stdio.h>

#define OK 0
#define ERR_GENERIC -1
#define ERR_INIT 1
#define ERR_SET 2
#define ERR_UNSET 3
#define ERR_TOKEN_LENGTH 4
#define ERR_INVALID_SYNTAX 5
#define ERR_UNKNOWN_COMMAND 6
#define ERR_ARGS 7
#define ERR_DIR 8
#define ERR_FILE 9

void print_error(int err_code, char *err_cause);

void exit_shell(int err_code, void *ptr);