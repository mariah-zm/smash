#include <regex.h>
#include <stdio.h>
#include <string.h>

#include "tokeniser.h"
#include "errors.h"
#include "variables.h"
#include "matcher.h"

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