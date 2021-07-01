#include "redirection.h"

#include <string.h>
#include "errors.h"

int input_redirection(token_t tokens[MAX_NUM_TOKENS], int count)
{
    int i = 0;

    while(i < count)
        if(strlen(tokens[i], "<")){
            if(i+1 == count)
                return ERR_SYNTAX;

            FILE *fp;

            // If file is not found, false is returned
            if((fp = fopen(file_name, "r")) == NULL){
                return ERR_FILE_NOT_FOUND;
            } else {
                token_t line;

                if(fgets(line, MAX_TOKEN_STRLEN, fp) == NULL)
                    return ERR_FILE_EMPTY

                if (fclose(fp) != 0)
                    print_error(ERR_FILE, "error closing file");

            }
        }
    }
}