#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "linenoise/linenoise.h"
#include "includes/tokeniser.h"
#include "includes/variables.h"
#include "includes/errors.h"
#include "includes/internal.h"
#include "includes/utils.h"
#include "includes/external.h"
#include "includes/redirection.h"

shell_var *shell_variables;
dirnode *head;

int prepare(char *line, token_t tokens[MAX_NUM_TOKENS], int *token_count);
void parse(token_t tokens[MAX_NUM_TOKENS], int token_count);
int variable_expansion(token_t tokens[MAX_NUM_TOKENS], int token_count);
int expand(char* string);
void execute(token_t *tokens, int start, int end);
void source(char *file_name);

int main(int argc, char **argv, char **envp)
{
    // Get input and echo here
    char *line;


    if (on_exit(exit_shell, NULL))
        perror("Cannot register exit handler\n");

    // Initialising shell variables on start up
    if((shell_variables = init_shell_vars(argv[0], envp)) == NULL)
        exit(ERR_INIT);

    head = init_dir(get_shell_var(shell_variables, "HOME"));
    pushdir(&head, get_shell_var(shell_variables, "CWD"));

    // Since this is a pointer, when the contents of the shell variable change, the output will also be updated
    char *prompt = get_shell_var(shell_variables, "PROMPT");

    while((line = linenoise(prompt)) != NULL) {

        // Reprompt for input if nothign was entered
        if(strcmp(line, "") == 0)
            continue;

        // Exit smash if 'exit' was entered
        if(strcmp(line, "exit") == 0){
            linenoiseFree(line);
            break;
        }

        token_t tokens[MAX_NUM_TOKENS]; 
        int token_count = 0;

        if(prepare(line, tokens, &token_count) == OK) {
            parse(tokens, token_count);

            reset(STDIN);
            reset(STDOUT);
        }

        // Freeing memory allocated by linenoise using linenoise
        linenoiseFree(line);
    }

    destroy_shell_vars(shell_variables);
    
    return OK;
}

int prepare(char *line, token_t tokens[MAX_NUM_TOKENS], int *token_count)
{
    if(strchr(line, '\"') != NULL && check_match(line, QUOTATION_PATTERN) == NOMATCH){
        print_error(ERR_INVALID_SYNTAX, "missing \"");
        return ERR_INVALID_SYNTAX;
    } else {
        tokenise(tokens, line, token_count, 0);

        if(variable_expansion(tokens, *token_count) == ERR_INVALID_SYNTAX)
            return ERR_INVALID_SYNTAX; // Error message already printed
        
        quote_removal(tokens, *token_count);

        // It is not allowed for the first token to be a metachar or control operator
        if(is_metachar_or_control_op(tokens[0])){
            char msg[50] = "unexpected token near \'";
            strcat(msg, tokens[0]);
            strcat(msg,  "\'");
            print_error(ERR_INVALID_SYNTAX, msg);
            return ERR_INVALID_SYNTAX;
        }
    }

    return OK;
}

void parse(token_t tokens[MAX_NUM_TOKENS], int token_count)
{
    if(check_match(tokens[0], ASSIGNMENT_PATTERN) == MATCH){
        char name[MAX_VAR_NAME_STRLEN];
        char value[MAX_VAR_VALUE_STRLEN];

        if(var_assignment(tokens[0], name, value) == ERR_INVALID_SYNTAX) 
            print_error(ERR_INVALID_SYNTAX, NULL);
        else
            insert_shell_var(shell_variables, name, value, false);
        
    } else {
        int in_redir, out_redir, pipe;

        if((out_redir = out_pos(tokens, token_count)) > 0){
            if(out_redir == token_count-1)
                print_error(ERR_INVALID_SYNTAX, "syntax error near unexpected token \'newline\'");
            else {
                int ret_code; 

                if(strcmp(tokens[out_redir+1], ">") == 0)
                    if(out_redir+1 == token_count-1)
                        print_error(ERR_INVALID_SYNTAX, "syntax error near unexpected token \'newline\'");
                    else
                        ret_code = redir_out(tokens, out_redir, APPEND);
                else 
                    ret_code = redir_out(tokens, out_redir, OVERWRITE);

                if(ret_code == OK){
                    parse(tokens, out_redir);
                } else
                    print_error(ret_code, "error redirecting");
            }
        } else if((in_redir = in_pos(tokens, token_count)) > 0){
            if(in_redir == token_count-1)
                print_error(ERR_INVALID_SYNTAX, "syntax error near unexpected token \'newline\'");
            else {
                if(redir_in(tokens, in_redir) ==  OK)
                    parse(tokens, in_redir);
                else                         
                    print_error(ERR_FILE_NOT_FOUND, "error redirecting");
            }
        } 

        // if contains pipe
        // handle pipe:
            
        if(in_redir == -1 && out_redir == -1)
            execute(tokens, 0, token_count-1);
    }
}

int variable_expansion(token_t tokens[MAX_NUM_TOKENS], int token_count)
{   
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

    if(expand_var(shell_variables, start_ptr, result) == ERR_INVALID_SYNTAX){
        print_error(ERR_INVALID_SYNTAX, result);
        return ERR_INVALID_SYNTAX;
    } else {
        strcpy(string, result);

        // Recursive case
        expand(string);
    }
}

void execute(token_t *tokens, int start, int end)
{
    int command = which_command(tokens[start]);
    // incrementing start pointer to skip command
    start++;

    switch(command){
        case ECHO:
            echo(tokens, start, end);
            break;
        case CD:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                if(cd(&head, tokens[1])){
                    update_cwd(shell_variables, tokens[1]);
                } else {
                    char err_msg[200] = "could not locate directory ";
                    strcat(err_msg, tokens[1]);
                    print_error(ERR_DIR, err_msg);
                }
            break;
        case SHOWVAR:
            if(end == 0)
                showvar(shell_variables, NULL);
            else if(end == start)
                showvar(shell_variables, tokens[1]);
            else 
                print_error(ERR_ARGS, "expected less than 1 arguments");
            break;
        case EXPORT:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                export_shell_var(shell_variables, tokens[1]);
            break;
        case UNSET:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                remove_shell_var(shell_variables, tokens[1]);
            break;
        case SHOWENV:
            if(end == 0)
                showenv(shell_variables, NULL);
            else if(end == start)
                showenv(shell_variables, tokens[1]);
            else 
                print_error(ERR_ARGS, "expected less than 1 arguments");
            break;
        case PUSHD:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else{
                if(pushdir(&head, tokens[1])){
                    update_cwd(shell_variables, head->dir);
                    dirs(head);
                } else {
                    char err_msg[200] = "could not locate directory ";
                    strcat(err_msg, tokens[1]);
                    print_error(ERR_DIR, err_msg);
                }
            }
            break;
        case POPD:
            if(end > 1)
                print_error(ERR_ARGS, "expected 0 arguments");
            else{
                popdir(&head);
                update_cwd(shell_variables, head->dir);
                dirs(head);
            }
            break;
        case DIRS:
            if(end > 1)
                print_error(ERR_ARGS, "expected 0 arguments");
            else
                dirs(head);
            break;
        case SOURCE:
            if(end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                source(tokens[1]);
            break;
        case EXTERNAL: 
            if(external(tokens, end+1, shell_variables) == ERR_UNKNOWN_COMMAND)
            {
                char err_msg[100];
                strcpy(err_msg, "command \'");
                strcat(err_msg, tokens[0]);
                strcat(err_msg, "\' not found");
                print_error(ERR_UNKNOWN_COMMAND, err_msg);
            }
            break;
    }
}

//file name either relative or absolute
void source(char *file_name)
{
    FILE *fp;

    // If file is not found, false is returned
    if((fp = fopen(file_name, "r")) == NULL)
    {
        char err_msg[200] = "could not locate the file ";
        strcat(err_msg, file_name);
        print_error(ERR_FILE_NOT_FOUND, err_msg);
    } else {
        char *line;

        while(fgets(line, MAX_TOKEN_STRLEN, fp) != NULL){
            token_t tokens[MAX_NUM_TOKENS];
            int token_count;
            if(prepare(line, tokens, &token_count) == OK)
                parse(tokens, token_count);
        }

        if (fclose(fp) != 0)
            print_error(ERR_FILE_NOT_FOUND, "error closing file");

    }
}
