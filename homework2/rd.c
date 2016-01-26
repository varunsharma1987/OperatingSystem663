#include "shell.h"
#include "cmd.h"
#include "util.h"

int noclobber;
int rd_enabled;
char rd_target[127];
int rd_type;
pid_t rd_in_pid;
pid_t rd_out_pid;

/* turn the redirection mode on */
void
rd_handler_on(pid_t process)
{
    /* in case ">" and ">&"*/
    if (rd_type == 1 || rd_type == 2) {
        /* target file exist */
        if (access(rd_target, W_OK) == 0) {
            if (noclobber == 1) {
                printf("Shell: error: unable to redirect because of noclobber constraints: cannot overwrite\n");
                return;
            }
        }
    }
    /* in case ">>" and ">>&" */
    if (rd_type == 3 || rd_type == 4) {
        /* target file doesn't exist */
        if (access(rd_target, W_OK) == -1) {
            if (noclobber == 1) {
                if (errno == ENOENT) {
                    printf("Shell: error: unable to redirect because of noclobber constraints: cannot create new file for appending\n");
                    return;
                }
            }
        }
    }
    /* switch the output file */
    if (rd_type < 5) {
        rd_out_pid = process;
        switch_out();
        return;
    }
    /* in case "<" */
    if (rd_type == 5) {
        rd_in_pid = process;
        if (access(rd_target, R_OK) == -1) {
            printf("Shell: rd: error: %s\n", strerror(errno));
            return;
        }
        switch_in_on();
    }
}

/* switch the output file to specified */
void
switch_out()
{
    int fid = -1;
    if (rd_type == 1 || rd_type == 2)
        fid = open(rd_target, O_WRONLY|O_CREAT|O_TRUNC, 0777);
    else if (rd_type == 3 || rd_type == 4)
        fid = open(rd_target, O_WRONLY|O_CREAT|O_APPEND, 0777);
    if (rd_type == 1 || rd_type == 3)
        close(1); /* stdout */
    else if (rd_type == 2 || rd_type == 4)
        close(2); /* stderr */
    dup(fid);
    close(fid);
    fchmod(fid, S_IRUSR|S_IRGRP|S_IROTH);
}

/* switch the input file to specified */
void
switch_in_on()
{
    int fid = -1;
    fid = open(rd_target, O_RDONLY, 0777);
    close(0);
    dup(fid);
    close(fid);
}

/* gets the file to operate when rd is enabled */
char*
get_rd_file(int argc, char** argv, int i)
{
    if (strcmp (argv[i], ">") == 0) 
        rd_type = 1;
    else if (strcmp (argv[i], ">&") == 0) 
        rd_type = 2;
    else if (strcmp (argv[i], ">>") == 0) 
        rd_type = 3;
    else if (strcmp (argv[i], ">>&") == 0) 
        rd_type = 4;
    else if (strcmp (argv[i], "<") == 0) 
        rd_type = 5;

    return (get_absolute_path(argv[argc - 1]));
}

/* checks whether command line contains redirects */
int
check_rd(int argc, char** argv)
{
    int i;
    int nullvalue = 0;
    struct stat st;
    rd_type = 0;
    for (i = 1; i <= argc - 2; i++) {
        if ( strncmp(argv[i], "<", 1) == 0 ||
                strncmp(argv[i], ">", 1) == 0 ) {
            if (argc - i != 2) {
                printf("Shell: rd: syntax error!\n");
                return SYNTAX_ERROR;
            }
            strcpy(rd_target, get_rd_file(argc, argv, i));
            return i;
        }
    }
    return nullvalue;
}

void
cmd_noclobber(int argc, char** argv)
{
    if (argc > 1) {
        printf("Shell: noclobber: syntax error!\n");
        return;
    }
    if (noclobber == 0) {
        noclobber = 1;
        printf("Shell: noclobber is ON now!\n");
    }
    else if (noclobber == 1) {
        noclobber = 0;
        printf("Shell: noclobber is OFF now!\n");
    }
    return;
}
