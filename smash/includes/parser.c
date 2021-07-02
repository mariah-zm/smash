#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "errors.h"
#include "matcher.h"
#include "executer.h"

void parse(char *line, dirnode *head, shell_var *shell_variables)
{
    int return_code;
    token_t tokens[MAX_NUM_TOKENS];

    if(strcmp(line, "") == 0)
        return;

    if(strchr(line, '\"') != NULL && check_match(line, QUOTATION_PATTERN) == NOMATCH)
        print_error(ERR_INVALID_SYNTAX, "missing \"");
    else {
        int token_count = 0;
        int *count_ptr = &token_count;

        tokenise(tokens, line, count_ptr, 0);

        if(variable_expansion(tokens, token_count, shell_variables) == ERR_INVALID_SYNTAX)
            return;
        
        quote_removal(tokens, token_count);

        // It is not allowed for the first token to be a metachar or control operator
        if(is_metachar_or_control_op(tokens[0])){
            char msg[50] = "unexpected token near \'";
            strcat(msg, tokens[0]);
            strcat(msg,  "\'");
            print_error(ERR_INVALID_SYNTAX, msg);
        } else {

        }
        
        if(check_match(tokens[0], ASSIGNMENT_PATTERN) == MATCH){
            char name[MAX_VAR_NAME_STRLEN];
            char value[MAX_VAR_VALUE_STRLEN];

            if(var_assignment(tokens[0], name, value) == ERR_INVALID_SYNTAX) 
                print_error(ERR_INVALID_SYNTAX, NULL);
            else
                insert_shell_var(shell_variables, name, value, false);
        
        } else {
            execute(tokens, 0, token_count-1, head, shell_variables);
        }
    }
}

void tokenise(token_t *tokens, char* input, int* index, int delim_level)
{   
    // Base case 
    if(strlen(input) == 0)
        return;

    // Base case 
    if(delim_level > 5){
        strcpy(tokens[*index], input);
        (*index)++;
        return;
    } 

    char delim = get_delim(delim_level);
    
    /* If the string does not contain the unquoted metacharacter, tokenisation is done
     * using the next metacharacter in the sequence as a delimeter
     */
    if(contains_delim(delim, input)) {
        char* right = strchr(input, delim);

        if(is_quoted(input, right)){
            char* next = strchr(right, '\"');
            right = strchr(next, delim);
        } else if(is_escaped(right)){
            right++;
            char* next = strchr(right, delim);
            right = next;
        }
        
        int end_left_index = strlen(input) - strlen(right);
        
        input[end_left_index] = '\0';

        // Tokenising left
        tokenise(tokens, input, index, delim_level+1);
    
        if(delim != ' ' && delim != '\t'){
            tokens[*index][0] = delim;
            tokens[*index][1] = '\0';
            (*index)++;
        }

        right++;
        // Tokenising right
        tokenise(tokens, right, index, delim_level);
    } else {
        tokenise(tokens, input, index, delim_level+1);
    }
}

char get_delim(int delim_lev)
{
    char delimeter;

    switch(delim_lev){
        case 0:
            delimeter = ';';
            break;
        case 1:
            delimeter = '|';
            break;
        case 2: 
            delimeter = '>';
            break;
        case 3: 
            delimeter = '<';
            break;
        case 4:
            delimeter = ' ';
            break;
        case 5: 
            delimeter = '\t';
            break;
    }

    return delimeter;
}

bool contains_delim(char delimeter, char* string)
{
    // Base case
    if(string == NULL)
        return false;

    char* ptr;

    if((ptr = strchr(string, delimeter)) == NULL){
        return false;
    } else {
        // If occurence is quoted or escaped then it is a literal value
        if(is_quoted(string, ptr)){
            char* new_ptr = strchr(ptr, '\"');
            // Checking for any other occurence past the quotation
            return contains_delim(delimeter, ++new_ptr);
        } else if(is_escaped(ptr)){
            return contains_delim(delimeter, ++ptr);
        } else {
            return true;
        }
    }
}

bool is_quoted(char* full_string, char* sub_string)
{
    if(sub_string == NULL)
        return false;

    // Only one quote exists before the sub-string, it means the sub string  
    return check_match(sub_string, "^[^\"]*[\"][^\"]*([^\"]*[\"][^\"]*[\"])*$");
}

bool is_escaped(char* ptr)
{
    if(ptr == NULL)
        return false;

    ptr--;

    if(*ptr == '\\')
        return true;
    else 
        return false;
}

int variable_expansion(token_t tokens[MAX_NUM_TOKENS], int token_count, shell_var *shell_variables)
{   
    int ret_code;
    char *start_ptr, *end_ptr;
    char result[MAX_VAR_VALUE_STRLEN];

    for(int i=0; i < token_count; i++){
        if(expand(tokens[i], shell_variables) == ERR_INVALID_SYNTAX)
            return ERR_INVALID_SYNTAX;
    }

    return OK;
}

int expand(char* string, shell_var *shell_variables)
{   
    char result[MAX_VAR_VALUE_STRLEN] = "\0";
    char *start_ptr = strchr(string, '$');

    // Base case
    if(start_ptr == NULL || is_escaped(start_ptr))
        return OK;

    if(expand_var(shell_variables, start_ptr, result) == ERR_INVALID_SYNTAX){
        print_error(ERR_INVALID_SYNTAX, result);
        return ERR_INVALID_SYNTAX;
    } else {
        strcpy(string, result);

        // Recursive case
        expand(string, shell_variables);
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

void remove_char(char*string, char to_remove)
{
    char* ptr = strchr(string, to_remove);
    char removed[MAX_TOKEN_STRLEN];

    // Base case
    if(ptr == NULL)
        return;

    if(!is_escaped(ptr)){
        int index = strlen(string) - strlen(ptr);
        strcpy(removed, ++ptr);
        string[index] = '\0';
    }
    
    strcat(string, removed);

    // Recursive call
    remove_char(string, to_remove);
}