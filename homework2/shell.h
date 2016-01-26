#ifndef H_SHELL
#define H_SHELL

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#define CMD_EMPTY -1
#define NORMAL 0
#define EXIT_SHELL -2
#define SYNTAX_ERROR -3
#define OTHER_ERROR -4

/* the PATH data structure */
struct pathelement
{
  char *element;
  struct pathelement *next;
};

/* the cmd data strucure */
struct cmd
{
  char *element;
  struct cmd *next;
};

/* the alias data structure */
struct alias
{
  char *new_name;
  char *old_name;
  struct alias *next;
};

/* the history data structure */
struct history
{
  char *cmd;
  struct history *next;
};

/* the history init function*/
void
history_init();

/* the history_add function */
struct history
*history_add();

/* the parse function for cmds,
 * the return value is "argc"
 */
int
parse_cmd();

/* find the command to execute */
int
find_cmd();

/* find the command to execute -- finding IPC*/
int
find_cmd_head();

/*the init function, initializes the basic environment of the shell*/
void 
shell_init();

/* This initializes the alias system */
void
alias_init();

/* This prints the PATH just gotten from system */
void
print_env_path();

/* get_path() function, used to get the current PATH variable*/
struct pathelement *get_path();

/* The respond cycle of the shell, 
 * from prompt to the finish of execution.
 * the return value indicates the status of the cycle.
 */ 
int 
respond_cycle();

/* prepare for next cycle
 * cleaning pointers and allocating new pointers
 * to prevent memory leak or some weird things
 * from happening
 */
void
prepare_for_next_cycle();

/* check_outer_cmd() function
 * checks whether command is an outer-source command
 */
int
check_outer_cmd(int, char**);

/* exec_fixed_path() function,
 * find fixed path outsource cmds and execute if matched
 */
int
exec_fixed_path(int, char**);

/* get_absolute_path() function,
 * do as its name shows...
 */
char
*get_absolute_path(char*);

/* The before_exit function. Deals with tracking the status of the
 * running shell and reports errors if any
 */
void
before_exit(int);

#endif
