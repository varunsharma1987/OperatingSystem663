#include "shell.h"
#include "cmd.h"

extern struct pathelement *path_tmp;
extern struct pathelement *path_list;
extern char *cwd;
extern char *prompt;
extern char **env;
extern char **env_tmp;
char *last_dir;

/* cmd_printenv() function,
 * gets the env variables of the current env
 */
int
cmd_printenv(int argc, char** argv, int mode)
{
  if (mode == 0)
    printf("Shell: Executing built-in command \"printenv\"\n");
  env_tmp = env;
  if (argc > 2) {
    printf("Shell: printenv: too many arguments!\n");
    return SYNTAX_ERROR;
  }
  else if (argc == 1) {
    while(*env_tmp != NULL) {
      printf("%s\n", *env_tmp);
      env_tmp++;
    }
  }
  else if (argc == 2) {
    char *ptr_path;
    ptr_path = getenv(argv[1]);
    if (ptr_path) {
      printf("%s=%s\n", argv[1], ptr_path);
    }
    else {
      printf("Shell: printenv: env var %s not found!\n", argv[1]);
      return OTHER_ERROR;
    }
  }
  return NORMAL;
}

/* cmd_setenv() function.
 * works the same as the tcsh built-in one.
 */
int
cmd_setenv(int argc, char** argv)
{
  printf("Shell: Executing built-in command \"setenv\"\n");
  if (argc > 3) {
    printf("Shell: setenv: too many arguments!\n");
    return SYNTAX_ERROR;
  }
  else if (argc == 1) {
    return cmd_printenv(argc, argv, 1);
  }
  else if (argc == 2) {
    char *ptr_path;
    ptr_path = getenv(argv[1]);
    if (ptr_path) {
      printf("Shell: setenv: failed to build a new env variable: %s already exists!\n", argv[1]);
      return OTHER_ERROR;
    }
    else {
      strcpy(ptr_path, argv[1]);
      strcat(ptr_path, "=");
      return putenv(ptr_path);
    }
  }
  else if (argc == 3) {
    char *ptr_path;
    ptr_path = getenv(argv[1]);
    strcat(ptr_path, "=");
    strcat(ptr_path, argv[2]);
    putenv(ptr_path);
    /* if user changed the HOME var, cd to HOME*/
    if (strcmp(argv[1], "HOME") == 0) {
        int fd;      
        char *new_dir = NULL;
        new_dir = getenv("HOME");
        if (chdir(new_dir) != 0) {
            printf("Shell: cd: error changing dir, %s\n", strerror(errno));
            return OTHER_ERROR;
        }
        else close(fd);
        strcpy(cwd, new_dir);
        return NORMAL;
    }
    /* if user changed the PATH var, reload */
    if (strcmp(argv[1], "PATH") == 0) {
      if (path_list) {
        path_tmp = path_list;
        while (path_tmp) {
          struct pathelement *next_to_del = path_tmp -> next;
          free(path_tmp);
          path_tmp = next_to_del;
        }
        path_list = (struct pathelement*)realloc(path_list, sizeof(struct pathelement));
      }
      path_list = get_path();
    }
  return NORMAL;
  }
}



