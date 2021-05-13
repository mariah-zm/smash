#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "parser.h"
#include "errors.h"
#include "variables.h"
#include "tokeniser.h"


// void parse(char* line)
// {
//     int return_code;
//     char result[MAX_TOKEN_STRLEN] = "\0";

//     token_t tokens[MAX_NUM_TOKENS];

//     if(line[0] == DOLLAR_SIGN){
//         if((return_code = expand_var(line, result)) != 0)
//             print_error(return_code, result);
//         else 
//             printf("%s\n", result);
//     } else 
//         if((return_code = tokenise(line)) != 0) {
//             char error[50];

//             snprintf(error, 50, "maximum token length is %d", MAX_TOKEN_STRLEN);

//             print_error(return_code, error);
//         }
// }

void parse(char* input)
{
    if(contains_metachar(NEWLINE, input)){

    } else if (contains_metachar(SEMICOLON, input)){

    } else if (contains_metachar(PIPE, input)) {

    } 
}

bool contains_metachar(char metachar, char* string)
{
    if(string == NULL)
        return false;

    char* ptr;

    if((ptr = strchr(string, metachar)) == NULL)
        return false;
    else {
        // If occurence is quoted or escaped then it is a literal value
        if(is_quoted(string, metachar) || is_escaped(ptr))
            return contains_metachar(metachar, ++ptr);
        else 
            return true;
    }
}

bool is_quoted(char* full_string, char metachar)
{
    char metachar_string[2];
    metachar_string[0] = metachar;
    metachar_string[1] = '\0';

    regex_t regex;
    char pattern[50];

    strcpy(pattern, "[^\"]*[\"][^\"]*[");
    strcat(pattern, metachar_string);
    strcat(pattern, "]+[^\"]*[\"][^\"]*");

    regcomp(&regex, pattern, REG_EXTENDED);

    int match = regexec(&regex, full_string, 0, NULL, 0);

    regfree(&regex);

    if(match == 0)
        return true;
    else
        return false;
}

bool is_escaped(char* ptr)
{
    ptr--;

    if(*ptr == BACKSLASH)
        return true;
    else 
        return false;
}

bool is_assignment(char* line)
{
    if(strchr(line, '=') == NULL)
        return false;
    else 
        return true;
}

void quote_removal(char* string, char to_remove)
{   
    char* ptr = strchr(string, to_remove);

    // Base case
    if(ptr == NULL)
        return;

    if(!is_escaped(ptr)){
        int index = strlen(string) - strlen(ptr);
        string[index] = '\0';
    }

    strcat(string, ++ptr);

    // Recursive call
    quote_removal(string, to_remove);
}