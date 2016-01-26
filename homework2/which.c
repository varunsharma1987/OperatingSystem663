#include "shell.h"
#include "cmd.h"

extern struct pathelement *path_tmp;
extern struct pathelement *path_list;
extern char *cwd;
extern char *prompt;
extern char **env;
extern char **env_tmp;
char *last_dir;

/* which() function, 
 * which finds the target file in env PATH
 * args:
 *   int argc: count of arguments (command arguments),
 *   char** argv: content of all arguments,
 *   int mode: mode of executing function
 *       1: single mode, used for cmd "which",
 *       2: exhausted mode, used for cmd "where",
 *       3: find mode, used for finding 
 *          non built-in commands.
 */
int 
cmd_which(int argc, char **argv, int mode, char* return_value)
{
    int reason = 0, fd, flag = 0;
    char* final_path = (char*)calloc(1, sizeof(char[255]));

    path_tmp = path_list;
    while(path_tmp != NULL) {
        final_path = (char*)realloc(final_path, sizeof(char[255]));
        memset(final_path, 0, 255);
        final_path = strcat(final_path, path_tmp->element);
        final_path = strcat(final_path, "/");
        if ((fd = open(final_path, O_RDONLY, 0)) == -1) {
            reason = access(final_path, X_OK);
            if (errno == EACCES && mode != 3) 
                printf("Shell: which: %s: permission denied!\n", final_path);
        }
        else close(fd);
        /* for the purpose of searching out-source commands,
         *  mode value is 3. */
        if (mode != 3)
            final_path = strcat (final_path, argv[1]);
        else
            final_path = strcat (final_path, argv[0]);
        if ((fd = open(final_path, O_RDONLY, 0)) != -1) {
            if (mode < 3)
                printf("%s\n", final_path);
            else if (mode == 3 && return_value != NULL) {
                strcpy(return_value, final_path);
            }
            flag++;
            close(fd);
            if (mode == 1 || mode == 3)
                return NORMAL;
        }
        path_tmp = path_tmp -> next;
    }
    if (mode == 1) 
        printf("Shell: which: no %s in PATH!\n", argv[1]);
    if (mode == 2 && flag == 0)
        printf("Shell: where: no %s in PATH!\n", argv[1]);
    if (mode == 2 && flag != 0)
        return NORMAL;
    return OTHER_ERROR;
}

