#include <glob.h>
#include "shell.h"
#include "cmd.h"
#include "util.h"

extern struct pathelement *path_tmp;
extern struct pathelement *path_list;
extern char *cwd;
extern char *prompt;
extern char **env;
extern char **env_tmp;
extern int pipe_enabled;
char *last_dir;

/* cmd_cd() function,
 * which changes the current directory
 */
int 
cmd_cd(int argc, char** argv)
{
  int fd, reason;      
  char *new_dir = (char*)calloc(1, sizeof(char[255]));
  char *temp = (char*)calloc(1, sizeof(char[255]));
  memset(temp, 0, 255);
  memset(new_dir, 0, 255);
  printf("Shell: Executing built-in command \"cd\"\n");
  strcpy(temp, cwd);
  if (argc > 2) {
    printf("Shell: cd: too many arguments! \n \
            Shell: cd: Usage: cd <directory> OR cd OR cd -\n");
    return SYNTAX_ERROR;
  }
  /* cd to home directory */
  if (argc == 1) {
    if ((new_dir=getenv("HOME")) == NULL) {
      printf("Shell: cd: unable to get home dir!\n");
      return OTHER_ERROR;
    }
  }
  else if (argc == 2) {
    if (strcmp(argv[1], "-") == 0) {
      if (strcmp(last_dir, "") == 0) {
        printf("Shell: cd: error: do not have last dir yet!\n");
        return OTHER_ERROR;
      }
      else {
        strcpy(new_dir, last_dir);
        //debug information
        printf("debug: last dir was %s\n", last_dir);
      }
    }
    else strcpy(new_dir, argv[1]);
  }

  /* Changes the dir */
  if (chdir(new_dir) != 0) {
    printf("Shell: cd: error changing dir, %s\n", strerror(errno));
    return OTHER_ERROR;
  }
  getcwd(cwd, 255);
  strcpy(last_dir, temp);
  return NORMAL;
}

/* is_wildcard() function
 * returns 0 if not wildcard, 1 if *, 2 if ?, 3 if both
 */
int
is_wildcard(char* input)
{
  int len = strlen(input);
  int i, qcount = 0, starcount = 0;
  for (i = 0; i < len; i++) {
    if (input[i] == '?')
        qcount++;
    else if (input[i] == '*')
        starcount++;
  }
  if (qcount > 0 && starcount > 0)
      return 3;
  else if (qcount > 0 && starcount == 0)
      return 2;
  else if (qcount == 0 && starcount > 0)
      return 1;
  else if (qcount == 0 && starcount == 0)
      return 0;
}

/* cmd_ls() function,
 * which functions the same as the quite common "ls"
 */
int
cmd_ls(int argc, char** argv)
{
  printf("Shell: Executing built-in command \"ls\"\n");
  int return_value;
  int wildcard = 0;
  DIR* curr_dir;
  struct stat st;
  char* dir_to_list;
  dir_to_list = (char*)calloc(1, sizeof(char[255]));
  /* in case only typing "ls" */
  if (argc == 1) {
    strcpy(dir_to_list, cwd);
    if ((curr_dir=opendir(dir_to_list)) == NULL) 
      printf("Shell: ls: error: %s\n", strerror(errno));
    return_value = ls_single(curr_dir);
  }
  /* in case of typing "ls sth" */
  else if (argc >= 2) {
    int i;
    for (i = 1; i < argc; i++) {
      stat(argv[i], &st);
      /* if sth is dir */
      if (S_ISDIR(st.st_mode)) {
        strcpy(dir_to_list, argv[i]);
        if ((curr_dir=opendir(dir_to_list)) == NULL) 
          printf("Shell: ls: error: %s\n", strerror(errno));
        printf("\nin %s: \n", get_absolute_path(argv[i]));
        return_value = ls_single(curr_dir);
      }
      /* if sth is not dir (a file) */
      else if (!S_ISDIR(st.st_mode)) {
        /* if doesn't contain wildcard */
        if (is_wildcard(argv[i]) == 0) {
          ls_file(argv[i]);
          return NORMAL;
        }
        /* If contains wildcard */
        else {
          ls_file_multi(wildcard_handler(argv[i]));
          return NORMAL;
        }
      }
    }
  }
  return NORMAL;
}

/* wildcard_handler() function
 */
struct wildcard
*wildcard_handler(char *input)
{
  struct wildcard *head = NULL, *tmp = NULL;
  glob_t glob_buffer;
  char* pattern = (char*)calloc(1, sizeof(char[255]));
  int i, match_count;

  if (strncmp(input, "/", 1) == 0 ||
      strncmp(input, "./", 2) == 0 ||
      strncmp(input, "../", 3) == 0) {
    pattern = get_absolute_path(input);
  }
  else {
    strcat(pattern, cwd);
    strcat(pattern, "/");
    strcat(pattern, input);
  }
  //debug information
  //printf("debug: wildcard pattern is %s\n", pattern);
  glob(pattern, 0, NULL, &glob_buffer); 
  match_count = glob_buffer.gl_pathc;

  for (i=0; i < match_count; i++) { 
    if (head == NULL) {
      head = (struct wildcard*)malloc(sizeof(struct wildcard));
      tmp = head;
      tmp->element = glob_buffer.gl_pathv[i];
      tmp -> next = NULL;
    }
    else {
      tmp -> next = (struct wildcard*)malloc(sizeof(struct wildcard));
      tmp = tmp -> next;
      tmp->element = glob_buffer.gl_pathv[i];
      tmp -> next = NULL;
    }
  }
  return head;
  globfree( &glob_buffer );
}

/* ls_file_multi() function
 * given a head of a linked list of filenames
 * do the ls_file
 */
void
ls_file_multi(struct wildcard *head)
{
  struct wildcard *tmp;
  tmp = head;
  while (tmp) {
    ls_file(tmp->element);
    tmp = tmp -> next;
  }
}

/* ls_file() function,
 * which prints the filename of a single file on the screen.
 */
void
ls_file(char* input)
{
  char* full_path = (char*)calloc(1, sizeof(char[255]));
  char* filename = (char*)calloc(1, sizeof(char[255]));
  if (strncmp(input, "./", 2) == 0 ||
      strncmp(input, "../", 3) == 0)  {
    strcpy(full_path, get_absolute_path(input));
  }
  /* this doesn't start with absolute path*/
  else if (strncmp(input, "/", 1) != 0){
    char* new_rela_path = (char*)calloc(1, sizeof(char[255]));
    strcpy(new_rela_path, "./");
    strcat(new_rela_path, input);
    full_path = get_absolute_path(new_rela_path);
  }
  /* This is absolute path */
  else
    strcpy(full_path, input);

  strcpy(filename, full_path);
  int i = strlen(full_path) - 1;
  while (filename[i] != '/') 
    i--;
  strncpy(full_path, filename, i);
  strcpy(filename, filename+i+1);
  //debug information
  //printf("debug: ls: path is %s, filename is %s\n", full_path, filename);

  DIR *dir_to_list = opendir(cwd);
  struct dirent *ptr;
  while ((ptr=readdir(dir_to_list)) != NULL) {
    if (strcmp(ptr->d_name, filename) == 0)
      printf("%s\n", ptr->d_name);
  }
}

/* ls_single() function,
 * which is used by the cmd_ls() function.
 * It only does the ls operation
 * for a single given directory.
 */
int
ls_single(DIR* dir_to_list)
{
  struct dirent *ptr;
  while ((ptr=readdir(dir_to_list)) != NULL) {
    printf("%s\n", ptr->d_name);
  }
  return NORMAL;
}

/* pwd() function,
 * which prints the current working directory
 */
void
cmd_pwd()
{
  printf("%s\n", cwd);
}



