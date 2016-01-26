#include "shell.h"
#include "cmd.h"
#include "util.h"
#include <sys/wait.h>

int pipe_enabled;
int left, right;
int pipefd[2];
int pipetype; /* 1: std; 2:err */
int leftpid;
int rightpid;
struct sigaction sigchld_action;
static siginfo_t sig_info;
sig_atomic_t child_exit_status;

/* find whether a pipe exists in a command */
int
find_pipe(char *cmd_char)
{
    int i;
    for (i = 0; i < strlen(cmd_char); i++) {
        if (cmd_char[i] == '|')
            return i;
    }
    return -1;
}

/* deals with pipes, adjusting the file descriptors 
 * NOTICE: this function runs in a just-created 
 *         child process and should not destroy the
 *         configs in the parent process! */
void
pipe_adjust_fd()
{
    /* the input end of the pipe */
    if (left == 1) {
        if (pipetype == 1) {
            close(1); /* Closing stdout */
            dup(pipefd[1]);  
            close(pipefd[0]);
        }
        else if (pipetype == 2) {
            close(2); /* Closing stderr */
            dup(pipefd[1]);
            close(1); /* Closing stdout */
            dup(pipefd[1]);
            close(pipefd[0]);
        }
    }
    /* the output end of the pipe */
    else if (right == 1) {
        close(0); /* Closing stdin */
        dup(pipefd[0]);
        close(pipefd[1]);
    }
}

/* cleans up the left process */
void
clean_up_child_process(int signal_number)
{
    int status;
    if (pipe_enabled == 1 && sig_info.si_pid == leftpid) {
        printf("left process exitted\n");
        if(waitpid(leftpid, &status, 0) < 0) {
            printf("Shell: in clean_up_child_process(): error: %s\n", strerror(errno));
            return;
        }
        close(pipefd[1]);
        leftpid = -1;
        child_exit_status = status;
    }
}

/* wait for the two processes before continue, in cycle.c */
void
wait_pipe_pid()
{
    pid_t to_wait;
    int option;
    int status;
    option = 0;

    if (left == 1) {
        waitpid(leftpid, &status, option);
        leftpid = -1;
        close(pipefd[1]);
    }
    else if (right == 1) {
        waitpid(rightpid, &status, option);
        rightpid = -1;
        close(pipefd[0]);
    }
    return;
}

