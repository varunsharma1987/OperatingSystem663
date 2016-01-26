#include "shell.h"
#include "cmd.h"
#include "util.h"
#include <sys/stat.h>
#include <sys/wait.h>

#define MAXBG 10

struct bg bg_list[MAXBG];
int bg_top;
int sid;

/* the initialization function for background execution */
void
bg_init(void)
{
  int i;
  for (i = 0; i < MAXBG; i++) {
    bg_list[i].pid = 0;
    bg_list[i].valid = 0;
  }
  bg_top = -1;
  sid = -1;
}

/* show all background jobs */
void
bg_show()
{
  printf("Shell: [current bg jobs]:\n");
  if (bg_top == -1) {
    printf("\tNo current background jobs!\n");
    return;
  }
  int i;
  for (i = bg_top; i >= 0; i--) {
    if (bg_list[i].valid = 1)
      printf("[ %d ]\tpid = %d\n", i+1, bg_list[i].pid);
  }
}

/* add a new background job to the list */
int
bg_add(pid_t pid_to_add)
{
  if (bg_top == MAXBG-1) {
    printf("Shell: error: too many background jobs!\n");
    return 1;
  }
  bg_top++;
  bg_list[bg_top].pid = pid_to_add;
  bg_list[bg_top].valid = 1;
  return 0;
}

/* remove a background job (will go to foreground) */
pid_t
bg_remove(int index_to_remove)
{
  if (bg_top == -1) {
    printf("Shell: error: no background job to remove!\n");
    return -1;
  }
  int i;
  pid_t pid_to_return = bg_list[index_to_remove].pid;
  bg_list[index_to_remove].valid = 0;

  if (index_to_remove == bg_top) {
      bg_list[index_to_remove].valid = 0;
  } else {
      for (i = index_to_remove; i < bg_top; i++) {
          bg_list[i].pid = bg_list[i+1].pid;
          bg_list[i].valid = bg_list[i+1].valid;
      }
  }
  bg_top--;
  return pid_to_return; 
}

/* cmd_fg() function.
 * with no arguments, it just put the latest bg job back;
 * with one argument, it put the process with the input
 *   job number back.
 */
int
cmd_fg(int argc, char** argv)
{
    if (argc > 2) {
        printf("Shell: fg: too many arguments!\n");
        return SYNTAX_ERROR;
    }
    else if (argc == 2) {
        if (atoi(argv[1]) > bg_top + 1) {
            printf("Shell: fg: no such job!\n");
            return OTHER_ERROR;
        } 
        else {
            int i = atoi(argv[1]) - 1;
            pid_t to_call = bg_list[i].pid;
            if (pid_exist(to_call)) {
                /* start to wait for the process */
                bg_remove(i);
                setpgid(to_call, getpgrp());
                if (waitpid(to_call, 0, 0) < 0) {
                    printf("Shell: error: waitpid error!\n");
                    return OTHER_ERROR;
                }
                return NORMAL;
            } 
            /* the process already somehow quit */
            else {
                printf("[ %d ]  pid = %d  Done.\n", i+1, to_call);
                return NORMAL;
            }
        }
    }
    else if (argc == 1) {
        if (bg_top == -1) {
            printf("Shell: no background jobs!\n");
            return OTHER_ERROR;
        }
        else {
            pid_t to_call = bg_list[bg_top].pid;
            if (pid_exist(to_call)) {
                /* start to wait for the process */
                bg_remove(bg_top);
                if (waitpid(to_call, 0, 0) < 0) {
                    printf("Shell: error: waitpid error!\n");
                    return OTHER_ERROR;
                }
                return NORMAL;
            } 
            /* the process already somehow quit */
            else {
                printf("[ %d ]  pid = %d  Done.\n", bg_top+1, to_call);
                return NORMAL;
            }
        }
    }
}

/* test whether a given process (PID) exists */
/* This is not done successfully yet! */
int
pid_exist(pid_t test)
{
  if (kill(test, 0) == -1) 
      return 0;
  else return 1;
}

/* check each pid's status in the bg list */
void
bg_checkall(void)
{
  //debug information
  //printf("debug: in bg_checkall() function\n");
  int i;
  if (bg_top == -1)
      return;
  for (i = 0; i <= bg_top; i++) {
    if ( waitpid(bg_list[i].pid, 0, WNOHANG) < 0) {
      printf("[ %d ] pid = %d  Done.\n", i, bg_list[i].pid);
      bg_remove(i);
    }
  }
}


