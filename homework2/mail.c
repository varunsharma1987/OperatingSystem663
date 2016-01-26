#include "shell.h"
#include "cmd.h"
#include "util.h"
#include <pthread.h>
#include <sys/time.h>

struct maillist {
    char filename[128];
    pthread_t tid;
    off_t st_size;
    struct maillist *next;
};
struct maillist *mailhead = NULL;

void
cmd_watchmail(int argc, char** argv)
{
    if (argc != 2 && argc != 3) {
        printf("Shell: error: syntax error!\n");
        return;
    }
    if (argc == 2) {
        watchmail_add(argv[1]);
        return;
    }
    if (argc == 3 && strcmp(argv[2], "off") == 0) {
        watchmail_remove(argv[1]);
        return;
    }
}


/* adds a watchmail option */
void
watchmail_add(char* to_add)
{
    struct maillist *tmp = find_mail_in_watch(to_add);
    if (tmp) {
        printf("Shell: watchmail: %s is already in watch!\n");
        return;
    }

    char filename[128];
    strcpy(filename, get_absolute_path(to_add));
    printf("Filename to watch is %s\n");
    if (access(filename, R_OK) == -1) {
        printf("Shell: matchmail: error: %s\n", strerror(errno));
        return;
    }

    tmp = (struct maillist*)malloc(sizeof(struct maillist));
    tmp->next = mailhead;
    mailhead = tmp;
    strcpy (tmp->filename, filename);
    struct stat st;
    stat(filename, &st);
    tmp->st_size = st.st_size;
    pthread_create(&(tmp->tid), NULL, mail_daemon, tmp);
}

/* remove a watchmail option */
void
watchmail_remove(char* to_remove)
{
    struct maillist *tmp = find_mail_in_watch(to_remove);
    struct maillist *tmp2 = mailhead;
    if (tmp == NULL) {
        printf("Shell: watchmail: %s is not in watch yet!\n");
        return;
    }
    if (mailhead == tmp) {
        mailhead = tmp->next;
    }
    else {
        while (tmp2) {
            if (tmp2->next == tmp)
                break;
            tmp2 = tmp2->next;
        }
        tmp2->next = tmp->next;
    }
    //debug information
    printf("the tid to cancel is : %d\n", tmp->tid);
    while (pthread_cancel(tmp->tid) != 0) {
        printf("Shell: watchmail: error: %s\n", strerror(errno));
    }
    free(tmp);
    return;
}

/* the daemon thread for watchmail */
void
*mail_daemon(void *in)
{
    char *to_watch = (char*)calloc(1, sizeof(char[255]));
    struct maillist *tmpmail = (struct maillist*) in;
    strcpy(to_watch, tmpmail->filename);
    printf("In daemon: to_watch is %s!\n", to_watch);
    struct stat st;
    struct timeval tv;
    while(1) {
        stat(tmpmail->filename, &st);
        if (tmpmail->st_size < st.st_size) {
            gettimeofday(&tv, NULL);
            printf("\n\aYou've got Mail in %s at %s\n", tmpmail->filename, ctime(&(tv.tv_sec)));
            tmpmail->st_size = st.st_size;
        }
        sleep(1);
    }
}

/* finds whether a mail is already in watch */
struct maillist *
find_mail_in_watch(char *to_find)
{
    struct maillist *tmp = mailhead;
    struct maillist *to_return = NULL;
    char filename[128];
    strcpy(filename, get_absolute_path(to_find));
    while (tmp) {
        if (strcmp(tmp->filename, filename) == 0) {
            to_return = tmp;
            break;
        }
        tmp = tmp->next;
    }
    return to_return;
}
