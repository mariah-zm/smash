#include "../redirection.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>

#include "../errors.h"

int saved_stdin, saved_stdout;
bool changed_in, changed_out;

int in_pos(token_t *tokens, int count)
{
    for(int i=0; i < count; i++){
        if(strcmp(tokens[i], "<") == 0){
            return i;
        }
    }

    return -1;
}

int out_pos(token_t *tokens, int count){
        for(int i=0; i < count; i++){
        if(strcmp(tokens[i], ">") == 0){
            return i;
        }
    }

    return -1;
}

void reset(int stream)
{
    if(stream == STDIN && changed_in){
        dup2(saved_stdin, 0);
        close(saved_stdin);
        changed_in = false;
    } else if(changed_out){
        dup2(saved_stdout, 1);
        close(saved_stdout);
        changed_out = false;
    }
}

int redir_in(token_t *tokens, int in_pos)
{
    char *file_name;
    file_name = tokens[in_pos + 1];

    int fd;
    fd = open(file_name, O_RDONLY, S_IRUSR);

    if (fd < 0)
        return ERR_GENERIC;

    saved_stdin = dup(0);

    dup2(fd, STDIN_FILENO);
    close(fd);

    changed_in = true;

    return OK;
}

int redir_out(token_t *tokens, int out_pos, int write_flag)
{
    char *file_name;
    int fd;

    if(write_flag == OVERWRITE){
        file_name = tokens[out_pos + 1];
        fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IXUSR);
    } else {
        file_name = tokens[out_pos + 2];
        fd = open(file_name, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR | S_IXUSR);
    }

    if (fd < 0)
        return ERR_GENERIC;

    saved_stdout = dup(1);

    dup2(fd, STDOUT_FILENO);
    close(fd);

    changed_out = true;

    return OK;
}

bool is_inredir()
{
    if(saved_stdin == 0)
        return false;
    else
        return true;
}