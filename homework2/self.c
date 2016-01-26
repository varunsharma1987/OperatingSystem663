#include "shell.h"
#include "cmd.h"

extern struct pathelement *path_tmp;
extern struct pathelement *path_list;
extern struct alias *alias_head;
extern struct history *hist_head;
extern struct history *hist_curr;
extern char *cwd;
extern char *prompt;
extern char **env;
extern char **env_tmp;
char *last_dir;

/* cmd_pid() function,
 * this prints the pid of the current shell
 * on the screen.
 */
void
cmd_pid(void)
{
  pid_t curr_pid;
  curr_pid = getpid();
  printf("Shell: pid: the pid of the current shell is %d!\n", curr_pid);
}

/* cmd_prompt() function,
 * this changes the prompt of the current shell.
 */
int
cmd_prompt(int argc, char** argv)
{
  if (argc > 2) {
    printf("Shell: prompt: too many arguments!\n\
            Shell: prompt: Usage: prompt <return>\n\
            Shell: prompt: prompt <new_prompt> <return>\n");
    return SYNTAX_ERROR;
  }
  else if (argc == 1) {
    printf("  input prompt prefix: ");
    gets(prompt);
  }
  else if (argc == 2) {
    strcpy(prompt, argv[1]);
  }
  return NORMAL;
}

/* cmd_alias() function.
 * still works the same as tsch built-in one.
 */
int
cmd_alias(int argc, char** argv)
{
  printf("Shell: Executing built-in command \"alias\"\n");
  if (argc != 2) {
    printf("Shell: alias: syntax error!\n\
            Shell: alias: Usage: alias oldname=newname\n");
    return SYNTAX_ERROR;
  }
  int index = 0, flag = 0 ;
  char tmp[255];
  strcpy(tmp, argv[1]);
  for (index = 0; index < strlen(tmp); index++) {
    if (tmp[index] == '=') {
      flag = 1;
      break;
    }
  }
  if (flag == 0 || tmp[0]=='=' || tmp[strlen(tmp)-1] == '=') {
    printf("Shell: alias: syntax error!\n\
            Shell: alias: Usage: alias oldname=newname\n");
    return SYNTAX_ERROR;
  }
  char* old_name = (char*)calloc(index, sizeof(char));
  strncpy(old_name, tmp, index);
  char* new_name = (char*)calloc(strlen(tmp)-index-1, sizeof(char));
  strncpy(new_name, tmp+index+1, strlen(tmp)-index-1);
  //debug information
  //printf("debug: alias: oldname was %s, newname is %s!\n", old_name, new_name);
  alias_new(new_name, old_name);
  return NORMAL;
}

/* alias_new() function,
 * this builds a new element in the
 * alias linked list.
 */
struct alias
*alias_new(char* new_name, char* old_name)
{
  struct alias *node, *tmp;
  if (alias_head->new_name == NULL) {
    node = alias_head;
  }
  else{
    node = (struct alias*)calloc(1, sizeof(struct alias));
    tmp = alias_head;
    while (tmp -> next) 
      tmp = tmp -> next;
    tmp -> next = node;
  }
  node->new_name = (char*)calloc(1, sizeof(new_name)); 
  node->old_name = (char*)calloc(1, sizeof(old_name));
  strcpy(node->new_name, new_name);
  strcpy(node->old_name, old_name);
  node->next = NULL;
  return node;
}

/* find_alias() function,
 * returns the old name for the alias new name.
 * If alias not found, return argv[0] itself.
 */
char
*find_alias(char* input)
{
  if(alias_head->new_name == NULL)
      return input;
  struct alias *iter = alias_head;
  int flag = 0;
  while(iter) {
    if (strcmp(iter->new_name, input) == 0) {
        //debug information
        printf("found old name %s for new name %s!\n", iter->old_name, iter->new_name);
        flag = 1;
        break;
    }
    iter = iter->next;
  }
  if (flag == 0)
      return input;
  else
      return iter->old_name;
}

/* cmd_history() function
 */
void
cmd_history(int argc, char** argv)
{
  printf("Shell: Executing built-in command \"history\"\n");
  if (argc > 2) {
    printf("Shell: history: too many arguments!\n\
            Shell: history: Usage: history <number>\n");
    return;
  }
  if (argc == 1) 
    print_history(10);
  else if (argc == 2) {
    print_history(atoi(argv[1]));
  }
}

/* print_history() function
 */
void
print_history(int number)
{
  struct history *iter = hist_head;
  int count = number;
  if (hist_head -> cmd == NULL) {
    printf("Shell: history: empty history.\n");
    return;
  }
  while(iter!=NULL && count > 0) {
    printf("%s\n", iter->cmd);
    count--;
    iter = iter -> next;
  }
}



