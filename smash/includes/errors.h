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
#define ERR_FILE_NOT_FOUND 9
#define ERR_FILE_EMPTY 10
#define ERR_PIPE 11

/**
 * The error handler for the shell.
 * @param err_code the code of the encountered error, corresponds to one of the macros defined above
 * @param err_cause a short description on what caused the error
 */  
void print_error(int err_code, char *err_cause);

/**
 * The exit handler for the shell.
 * @param err_code the exit code 
 */ 
void exit_shell(int err_code, void *ptr);