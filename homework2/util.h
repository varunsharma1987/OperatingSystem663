#ifndef H_UTIL
#define H_UTIL
#include <assert.h>
/* the data structure for background jobs */
struct bg {
  pid_t pid;
  int valid;
};

/* the initialization function for background execution */
void 
bg_init(void);

/* add a new background job to the list */
int
bg_add(pid_t);

/* remove a background job (will go to foreground)*/
pid_t
bg_remove(int);

/* show all background jobs */
void
bg_show();

/* test whether a given process (PID) exists */
int
pid_exist(pid_t);

/* check each pid's status in the bg list */
void
bg_checkall();

/* check the load of the system and action if necessary */
static void
*checkload();

/* gets the system load for 1 min */
int 
get_load(double*);

/* gets the currently logged-in users */
void
get_all_users(void);

/* adds a watchmail option */
void
watchmail_add(char*);

/* the daemon thread for watchmail */
void
*mail_daemon(void *);

/* remove a watchmail option */
void
watchmail_remove(char*);

/* finds whether a mail is already in watch */
struct maillist *
find_mail_in_watch(char *);

/* checks whether command line contains redirects */
int
check_rd(int, char**);

/* gets the file to operate when rd is enabled */
char*
get_rd_file(int, char**, int);

/* turn the redirection mode on */
void
rd_handler_on(pid_t);

/* switch the output file to specified */
void
switch_out();

/* switch the input file to specified */
void
switch_in_on();

/* find whether a pipe exists in a command */
int
find_pipe(char*);

/* deals with pipes, adjusting the file descriptors */
void
pipe_adjust_fd();

/* wait for the pipe processes before continue, in cycle.c */
void
wait_pipe_pid();

/* cleans up the left process */
void
clean_up_child_process(int);
#endif
