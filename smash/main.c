#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

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
int pipe_command(token_t *tokens, int pipe_pos, int token_count);

int main(int argc, char **argv, char **envp)
{
    // Get input and echo here
    char *line;

    // Registering error handler
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

        // Parsing only if 
        if(prepare(line, tokens, &token_count) == OK)
            parse(tokens, token_count);

        reset(STDIN);
        reset(STDOUT);

        // Freeing memory allocated by linenoise using linenoise
        linenoiseFree(line);
    }

    destroy_shell_vars(shell_variables);
    free_dirs(&head);
    
    return OK;
}

int prepare(char *line, token_t tokens[MAX_NUM_TOKENS], int *token_count)
{
    if(strchr(line, '\"') != NULL && check_match(line, QUOTATION_PATTERN) == NOMATCH){
        print_error(ERR_INVALID_SYNTAX, "missing \"");
        return ERR_INVALID_SYNTAX;
    }
    
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

    // Last token cannot be a redirection symbol
    if(strcmp(tokens[*token_count-1], ">") == 0 || strcmp(tokens[*token_count-1], "<") == 0) {
        print_error(ERR_INVALID_SYNTAX, "syntax error near unexpected token \'newline\'");
        return ERR_INVALID_SYNTAX;
    }

    return OK;
}

void parse(token_t tokens[MAX_NUM_TOKENS], int token_count)
{
    // Checking if input was an assignment, in which case the variable is set
    if(check_match(tokens[0], ASSIGNMENT_PATTERN) == MATCH){
        char name[MAX_VAR_NAME_STRLEN];
        char value[MAX_VAR_VALUE_STRLEN];

        if(var_assignment(tokens[0], name, value) == ERR_INVALID_SYNTAX) 
            print_error(ERR_INVALID_SYNTAX, NULL);
        else
            insert_shell_var(shell_variables, name, value, false);
        
    } else {
        // Hold the position of redir and pipe symbols, if any
        int semi_c, pipe, in_redir, out_redir;

        // Checking for semi-colons first
        if((semi_c = get_pos(tokens, token_count, ";")) > 0){
            // Executing LHS of ;
            parse(tokens, semi_c);
            // Executing RHS of ;
            parse(tokens+(semi_c+1), token_count-semi_c-1);

        // Checking for pipes    
        }else if((pipe = get_pos(tokens, token_count, "|")) > 0){
            if(pipe_command(tokens, pipe, token_count) != OK)
                print_error(ERR_PIPE, "error piping commands");

        // Checking for output redirection
        } else if((out_redir = get_pos(tokens, token_count, ">")) > 0){
            int ret_code; 

            if(strcmp(tokens[out_redir+1], ">") == 0)
                ret_code = redir_out(tokens, out_redir, APPEND);
            else 
                ret_code = redir_out(tokens, out_redir, OVERWRITE);

            if(ret_code == OK)
                parse(tokens, out_redir);
            else {
                print_error(ERR_FILE_NOT_FOUND, "could not find file or directory");
                return;
            }
        
        // Checking for input redirection
        } else if((in_redir = get_pos(tokens, token_count, "<")) > 0){
            if(redir_in(tokens, in_redir) ==  OK)
                parse(tokens, in_redir);
            else {                         
                print_error(ERR_FILE_NOT_FOUND, "could not find file or directory");
                return;
            }

        // Otherwise execute normally
        } else if (token_count != 0){
            execute(tokens, 0, token_count-1);
        }
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
    // Incrementing start pointer to skip command
    start++;

    token_t line;
    bool redir = false;

    if((redir = is_inredir()) && command != ECHO) {
        fgets(line, MAX_TOKEN_STRLEN, stdin);
        if(line[strlen(line)-1] == '\n')
            line[strlen(line)-1] = '\0';
        
        strcpy(tokens[1], line);

        reset(STDIN);
    }

    switch(command){
        case ECHO:
            echo(tokens, start, end);
            break;

        case CD:
            if(!redir && end != start)
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
            if(redir)
                showvar(shell_variables, tokens[1]);
            else if(end == 0)
                showvar(shell_variables, NULL);
            else if(end == start)
                showvar(shell_variables, tokens[1]);
            else 
                print_error(ERR_ARGS, "expected less than 1 arguments");
            break;

        case EXPORT:
            if(!redir && end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                export(shell_variables, tokens[1]);
            break;

        case UNSET:
            if(!redir && end != start)
                print_error(ERR_ARGS, "expected 1 argument");
            else
                unset(shell_variables, tokens[1]);
            break;

        case SHOWENV:
            if(!redir && end == 0)
                showenv(shell_variables, NULL);
            else if(!redir && end == start)
                showenv(shell_variables, tokens[1]);
            else 
                print_error(ERR_ARGS, "expected less than 1 arguments");
            break;

        case PUSHD:
            if(!redir & end != start) 
                print_error(ERR_ARGS, "expected 1 argument");
            else
                if(pushdir(&head, tokens[1])){
                    update_cwd(shell_variables, head->dir);
                    dirs(head);
                } else {
                    char err_msg[200] = "could not locate directory ";
                    strcat(err_msg, tokens[1]);
                    print_error(ERR_DIR, err_msg);
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
            if(!redir && end != start) 
                print_error(ERR_ARGS, "expected 1 argument");
            else 
                source(tokens[1]);
            
            break;

        case EXTERNAL: 
            if(external(tokens, end+1, shell_variables) == ERR_UNKNOWN_COMMAND)            {
                char err_msg[100];
                strcpy(err_msg, "command \'");
                strcat(err_msg, tokens[0]);
                strcat(err_msg, "\' not found");
                print_error(ERR_UNKNOWN_COMMAND, err_msg);
            }
            break;
    }
}

// File name either relative or absolute
void source(char *file_name)
{
    FILE *fp;

    if((fp = fopen(file_name, "r")) == NULL){
        char err_msg[200] = "could not locate the file ";
        strcat(err_msg, file_name);
        print_error(ERR_FILE_NOT_FOUND, err_msg);
    } else {
        token_t line;

        /* This while loop performs what the main loop in the main function 
         * does but reads the input from the given file instead of linenoise
         */
        while(fgets(line, MAX_TOKEN_STRLEN, fp) != NULL){
            if(line[strlen(line)-1] == '\n')
               line[strlen(line)-1] = '\0';

            token_t tokens[MAX_NUM_TOKENS];
            int token_count = 0;

            if(prepare(line, tokens, &token_count) == OK)
                parse(tokens, token_count);
            
            reset(STDIN);
            reset(STDOUT);
        }

        if (fclose(fp) != 0)
            print_error(ERR_FILE_NOT_FOUND, "error closing file");

    }
}

int pipe_command(token_t *tokens, int pipe_pos, int token_count)
{
    int fd[2];

    if(pipe(fd) != 0)
        return ERR_PIPE;

    // Forking for LHS
    pid_t pid_lhs = fork();

    if(pid_lhs < 0){
        return ERR_PIPE;
    } else if(pid_lhs == 0){
        // Setting write-end
        dup2(fd[1], STDOUT_FILENO);
        // Closing read-end
        close(fd[0]);

        parse(tokens, pipe_pos);
        _Exit(EXIT_SUCCESS);
    } 

    // Forking for RHS
    pid_t pid_rhs = fork();

    if(pid_rhs < 0){
        return ERR_PIPE;
    } else if (pid_rhs == 0){
        // Setting read-end
        dup2(fd[0], STDIN_FILENO);
        // Closing write-end
        close(fd[1]);
        
        parse(tokens+(pipe_pos+1), token_count-pipe_pos-1);
        _Exit(EXIT_SUCCESS);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid_lhs, NULL, WUNTRACED);
    waitpid(pid_rhs, NULL, WUNTRACED);

    return OK;
}