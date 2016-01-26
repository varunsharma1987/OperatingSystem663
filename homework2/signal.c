#include "shell.h"
#include "cmd.h"
#include "sighand.h"
#include <signal.h>
#include <sys/types.h>

extern struct pathelement *path_tmp;
extern struct pathelement *path_list;
extern char *cwd;
extern char *prompt;
extern char **env;
extern char **env_tmp;
char *last_dir;


/* the signal handler for SIGINT */
void
sigint_handler()
{
  printf("SIGINT caught!\n");
}

/* cmd_kill() function
 */
int
cmd_kill(int argc, char** argv)
{
  printf("Shell: Executing built-in command \"kill\"\n");
  if (argc > 3 || argc == 1) {
    printf("Shell: kill: syntax error!\n\
            Shell: kill: Usage: kill <pid>, kill -<SIGNAL> <pid>\n");
    return SYNTAX_ERROR;
  }
  if (argc == 2) {
    /* color egg */
    if (strcmp(argv[1], "gaozu") == 0) {
      printf("It is dead!!!\n");
      return NORMAL;
    }
    if (kill(atoi(argv[1]), SIGTERM) == -1) {
      printf("Shell: kill: error: %s", strerror(errno));
      return OTHER_ERROR;
    }
  }
  else if (argc == 3) {
    if (*argv[1] != '-') {
        printf("Shell: kill: syntax error!\n\
                Shell: kill: Usage: kill <pid>, kill -<SIGNAL> <pid>\n");
        return SYNTAX_ERROR;
    }
    else {
      char sig[5];
      int to_kill;
      strcpy(sig, argv[1]+1);
      if (strcmp(argv[2], "pid-of-shell") == 0) 
          to_kill = getpid();
      else
          to_kill = atoi(argv[2]);
      if (kill(to_kill, atoi(sig)) == -1) {
          printf("Shell: kill: error: %s\n", strerror(errno));
          return OTHER_ERROR;
      }
      return NORMAL;
    }
  }
}
