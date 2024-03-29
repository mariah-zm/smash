#include "../tokeniser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include "../errors.h"
#include "../utils.h"

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
     * using the next metacharacter in the sequence as a delimiter
     */
    if(contains_delim(delim, input)) {
        char* right = strchr(input, delim);

        // If this particular occurrence is quoted/escaped, go to the next one
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
    switch(delim_lev){
        case 0:
            return ';';
        case 1:
            return '|';
        case 2: 
            return '>';
        case 3: 
            return '<';
        case 4:
            return ' ';
        case 5: 
            return '\t';
    }
}

bool contains_delim(char delimiter, char* string)
{
    char* ptr;

    if((ptr = strchr(string, delimiter)) == NULL){
        return false;
    } else {
        // If occurence is quoted or escaped then it is a literal value
        if(is_quoted(string, ptr)){
            char* new_ptr = strchr(ptr, '\"');
            // Checking for any other occurence past the quotation
            return contains_delim(delimiter, ++new_ptr);
        } else if(is_escaped(ptr)){
            return contains_delim(delimiter, ++ptr);
        } else {
            return true;
        }
    }
}