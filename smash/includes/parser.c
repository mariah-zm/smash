#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "parser.h"
#include "errors.h"
#include "utils.h"
#include "variables.h"

void parse(char *line)
{
    int return_code;
    token_t tokens[MAX_NUM_TOKENS];

    if(strchr(line, '\"') != NULL && !is_matched(line, QUOTATION_PATTERN))
        print_error(ERR_INVALID_SYNTAX, "missing \"");
    else {
        int token_count = 0;
        int *count_ptr = &token_count;

        tokenise(tokens, line, count_ptr, 0);

        if(variable_expansion(tokens, token_count) == ERR_INVALID_SYNTAX)
            return;
        
        quote_removal(tokens, token_count);

        if(is_assignment(tokens[0])){
            if(set_shell_var(tokens[0]) == ERR_INVALID_SYNTAX) 
                print_error(ERR_INVALID_SYNTAX, NULL);
        } else if(is_metachar_or_control_op(tokens[0])){
            char msg[50] = "unexpected token near \'";
            strcat(msg, tokens[0]);
            strcat(msg,  "\'");
            print_error(ERR_INVALID_SYNTAX, msg);
        } else {
            printf("Not a valid command\n");
        }
    }
}

int variable_expansion(token_t tokens[MAX_NUM_TOKENS], int token_count)
{   
    int ret_code;
    char *start_ptr, *end_ptr;
    char result[MAX_VAR_VALUE_STRLEN];

    for(int i=0; i < token_count; i++){
        if(expand(tokens[i]) == ERR_INVALID_SYNTAX)
            return ERR_INVALID_SYNTAX;
    }

    return OK;
}

int expand(char* string)
{   
    char result[MAX_VAR_VALUE_STRLEN] = "\0";
    char *start_ptr = strchr(string, '$');

    // Base case
    if(start_ptr == NULL || is_escaped(start_ptr))
        return OK;

    if(expand_var(start_ptr, result) == ERR_INVALID_SYNTAX){
        print_error(ERR_INVALID_SYNTAX, result);
        return ERR_INVALID_SYNTAX;
    } else {
        strcpy(string, result);

        // Recursive case
        expand(string);
    }

}

void quote_removal(token_t tokens[MAX_NUM_TOKENS], int token_count)
{   
    for(int i=0; i < token_count; i++){
        remove_char(tokens[i], '\"');
        remove_char(tokens[i], '\\');
    }
}

bool is_assignment(char* string)
{
    if(strchr(string, '=') == NULL)
        return false;
    else 
        return true;
}

bool is_metachar_or_control_op(char *string)
{
    if(strcmp(string, ";") == 0
        || strcmp(string, "|") == 0
        || strcmp(string, "<") == 0
        || strcmp(string, ">") == 0
        || strcmp(string, "\n") == 0)
        return true;
    else
        return false;
}