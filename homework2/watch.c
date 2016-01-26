#include "shell.h"
#include "cmd.h"
#include "util.h"
#include <utmpx.h>
#include <pthread.h>

struct utmpx *up;
pthread_t user_tid = 0;
pthread_mutex_t watchlist_mutex_lock = PTHREAD_MUTEX_INITIALIZER;

struct userlist {
    struct utmpx element;
    struct userlist *next;
};
struct userlist *user_head = NULL;
struct userlist *tmp = NULL;
struct userlist *tmp2 = NULL;

int watchlist_count;
struct watchlist {
    char name[255];
    struct userlist *head;
    int count;
    struct watchlist *next;
};
struct watchlist *watchlist_head;

void
show_all_users(void);

/* the daemon for watching users */
void
*watch_user_daemon(void);

/* init function for watchlist */
void
watchlist_init();

/* add new element to watchlist */
void
watchlist_add(char*);

/* remove element from watchlist */
void
watchlist_remove(char*);

/* cmd_watchuser() function. */
void
cmd_watchuser(int argc, char **argv)
{
    if (argc > 3) {
        printf("Shell: watchuser: syntax error!\n");
        return;
    }
    /* start the thread if not exist */
    if (user_tid == 0) {
        pthread_mutex_lock(&watchlist_mutex_lock);
        watchlist_init();
        pthread_mutex_unlock(&watchlist_mutex_lock);
        pthread_create(&user_tid, NULL, watch_user_daemon, "watchuser");
    }
    /* watchuser username */
    if (argc == 2) {
        pthread_mutex_lock(&watchlist_mutex_lock);
        watchlist_add(argv[1]);
        pthread_mutex_unlock(&watchlist_mutex_lock);
        return;
    }
    /* watchuser username off */
    if (argc == 3) {
        if (strcmp (argv[2], "off") == 0) {
            pthread_mutex_lock(&watchlist_mutex_lock);
            watchlist_remove(argv[1]);
            pthread_mutex_unlock(&watchlist_mutex_lock);
        }
    }
}

void
watchlist_add(char* to_add)
{
    struct watchlist *tmp;
    tmp = watchlist_head;
    while (tmp) {
        if (strcmp(to_add, tmp->name) == 0) {
            printf("Shell: username %s already in watch!\n", to_add);
            return;
        }
        tmp = tmp->next;
    }
    /* to_add is not in watch yet, critical section starts*/
    tmp = (struct watchlist*)malloc(sizeof(struct watchlist));
    strcpy (tmp->name, to_add);
    tmp->head = NULL;
    tmp->count = -1;
    if (watchlist_head == NULL) {
        watchlist_head = tmp;
        watchlist_head -> next = NULL;
    }
    else {
        tmp -> next = watchlist_head;
        watchlist_head = tmp;
    }
    printf("Added user to watch: %s\n", to_add);
    /* added, critical section over */
}

void
watchlist_remove(char* to_remove)
{
    struct watchlist *tmp, *tmplast = NULL;
    tmp = watchlist_head;
    int flag = 0;
    /* search */
    while (tmp) {
        if (strcmp(to_remove, tmp->name) == 0) {
            flag = 1;
            break;
        }
        if (tmp->next) {
            tmplast = tmp;
            tmp = tmp->next;
        }
    }
    if (flag = 0) {
        printf("Shell: there's no user called %s in watch!", to_remove);
        return;
    }
    /* remove, critical section start */
    if (tmplast == NULL) {
        free(tmp);
        watchlist_head = NULL;
    }
    else {
        tmplast->next = tmp->next;
        free(tmp);
    }
    watchlist_count--;
    /* critical section over */
}

/* adds a record to a watchlist element */
void
ut_insert(struct watchlist *tmpwatch,
            struct utmpx to_insert)
{
    struct userlist *tmp = (struct userlist*)malloc(sizeof(struct userlist));
    tmp->next = tmpwatch->head;
    tmpwatch->head = tmp;

    assert(tmpwatch->head);
    /* doing the first time copy */
    tmp->element = to_insert;
}

/* deletes a record from a watchlist element */
void
ut_remove(struct watchlist *tmpwatch,
            struct userlist *to_remove)
{
    struct userlist *tmp = tmpwatch->head;
    struct userlist *tmp2;
    while(tmp) {
        if (tmp == to_remove) {
            if (tmp2 == NULL) {
                (tmpwatch->head)->next = tmp->next;
                free(tmp);
            }
            else {
                tmp2->next = tmp->next;
                free(tmp);
            }
            break;
        }
        tmp2 = tmp;
        tmp = tmp -> next;
    }
}

/* Do the initialization for a new watched user */
void
new_user_init(struct watchlist *tmpwatch)
{
    struct userlist* tmpuser = user_head;
    tmpwatch->head = NULL;
    while (tmpuser) {
        /* match user */
        if (strcmp(tmpwatch->name, (tmpuser->element).ut_user) == 0) {
            ut_insert(tmpwatch, tmpuser->element);
            //debug information
            printf("inserted 1 for new user\n");
        }
        tmpuser = tmpuser->next;
    }
    if (tmpwatch->count == -1)
        tmpwatch->count = 1;
}

/* search for new logins */
void
scan_for_new_login(struct watchlist *tmpwatch)
{
    struct userlist *tmpu = user_head;
    struct userlist *tmpw = tmpwatch->head;
    int flag = 0;
    while(tmpu) {
        if (strcmp((tmpu->element).ut_user, tmpwatch->name) != 0) {
            tmpu = tmpu -> next;
            continue;
        }
        flag = 0;
        tmpw = tmpwatch->head;
        while(tmpw) {
            //debug information
            //printf("in tmpw: u.user=%s, w.user=%s; u.line=%s, w.line=%s; u.host=%s, w.host=%s\n", (tmpw->element).ut_user, (tmpu->element).ut_user, (tmpw->element).ut_line, (tmpu->element).ut_line, (tmpw->element).ut_host, (tmpu->element).ut_host);

            if (strcmp((tmpw->element).ut_user, (tmpu->element).ut_user) == 0 &&
                    strcmp((tmpw->element).ut_line, (tmpu->element).ut_line) == 0 &&
                    strcmp((tmpw->element).ut_host, (tmpu->element).ut_host) == 0) {
                flag = 1;
                break;
            }
            tmpw = tmpw->next;
        }
        if (flag == 0) {
            printf("Shell: watchuser: %s has logged on %s from %s!\n", 
                    (tmpu->element).ut_user,
                    (tmpu->element).ut_line,
                    (tmpu->element).ut_host);
        }
        tmpu = tmpu->next;
    }
}

/* search for new exit */
void
scan_for_new_exit(struct watchlist* tmpwatch)
{
    struct userlist *tmpu = user_head;
    struct userlist *tmpw = tmpwatch->head;
    int flag = 0;
    while(tmpw) {
        flag = 0;
        tmpu = user_head;
        while(tmpu) {
            if (strcmp((tmpu->element).ut_user, tmpwatch->name) != 0) {
                tmpu = tmpu -> next;
                continue;
            }
            if (strcmp((tmpw->element).ut_user, (tmpu->element).ut_user) == 0 &&
                    strcmp((tmpw->element).ut_line, (tmpu->element).ut_line) == 0 &&
                    strcmp((tmpw->element).ut_host, (tmpu->element).ut_host) == 0) {
                flag = 1;
                break;
            }
            tmpu = tmpu->next;
        }
        if (flag == 0) {
            printf("Shell: watchuser: %s has logged off %s from %s!\n", 
                    (tmpw->element).ut_user,
                    (tmpw->element).ut_line,
                    (tmpw->element).ut_host);
        }
        tmpw = tmpw->next;
    }
}

/* Do the comparison for old entries */
void
compare_old(struct watchlist *tmpwatch)
{
    scan_for_new_login(tmpwatch);
    scan_for_new_exit(tmpwatch);
}

/* Compare the value of watched users and assign new values */
void
compare_value()
{
    char usernow[32];
    struct watchlist* tmpwatch = watchlist_head;
    while(tmpwatch) {
        strcpy (usernow, tmpwatch -> name);
        /* If the user was just added into watchlist, first time scan */
        if (tmpwatch->count == -1) {
            new_user_init(tmpwatch);
        }
        /* Scan old entry for existence and compare the value */
        else compare_old(tmpwatch); 
        tmpwatch = tmpwatch->next;
    }
}

/* copy the new status to watchlist */
void
adjust_watchlist()
{
    struct watchlist *tmpwatch = watchlist_head;
    struct userlist *tmpuser = user_head;
    while (tmpwatch) {
        tmpwatch->head = NULL;
        while (tmpuser) {
            if (strcmp((tmpuser->element).ut_user, tmpwatch->name) == 0)
                ut_insert (tmpwatch, tmpuser->element);
            tmpuser = tmpuser->next;
        }
        tmpwatch = tmpwatch->next;
    }
}

void
*watch_user_daemon(void)
{
    while(1) {
        user_head = NULL;
        get_all_users();
        //show_all_users();
        pthread_mutex_lock(&watchlist_mutex_lock);
        compare_value();
        adjust_watchlist();
        pthread_mutex_unlock(&watchlist_mutex_lock);
        sleep(2);
    }
}

void
watchlist_init()
{
    watchlist_count = 0;
    watchlist_head = NULL;
}

/* gets the currently logged-in users */
void
get_all_users()
{
    setutxent();
    while (up = getutxent() ) {
        if (up -> ut_type == USER_PROCESS) {
            tmp = (struct userlist*)malloc(sizeof(struct userlist));
            if (user_head == NULL) {
                tmp->next = NULL;
                user_head = tmp;
            }
            else {
                tmp->next = user_head;
                user_head = tmp;
            }
            tmp -> element = *up;
        }
    }
}

void
show_all_users()
{
    tmp = user_head;
    while(tmp) {
        printf("%s has logged on %s from %s\n", (tmp->element).ut_user, (tmp->element).ut_line, (tmp->element).ut_host);
        tmp = tmp->next;
    }
}
