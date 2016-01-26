#include "shell.h"
#include "util.h"
#include <pthread.h>
#include <sys/param.h>

#ifdef HAVE_KSTAT
#include <kstat.h>
#endif

double load_thold;
pthread_t tid = 0;
double load;

/* cmd_warnload() function.
*/
void
cmd_warnload(int argc, char** argv)
{
    if (argc != 2) {
        printf("Shell: Syntax error: Usage: warnload <value>\n");
        return;
    }
    load_thold = atof(argv[1]);
    if (tid == 0)
        pthread_create(&tid, NULL, checkload, NULL);
    return;
}

static void
*checkload()
{
    while(1) {
        if (get_load(&load) == -1) {
            printf("Shell: get_load() failure!\n");
            pthread_exit(NULL);
        }
        if (load > load_thold) 
            printf("Shell: Warning: Load level is %f!\n", load_thold);
        sleep(3);
    }
    pthread_exit(NULL);
}

int 
get_load(double *loads)
{
#ifdef HAVE_KSTAT
    kstat_ctl_t *kc;
    kstat_t *ksp;
    kstat_named_t *kn;

    kc = kstat_open();
    if (kc == 0) {
        perror("kstat_open");
        exit(1);
    }

    ksp = kstat_lookup(kc, "unix", 0, "system_misc");
    if (ksp == 0) {
        perror("kstat_lookup");
        exit(1);
    }
    if (kstat_read(kc, ksp,0) == -1) {
        perror("kstat_read");
        exit(1);
    }

    kn = kstat_data_lookup(ksp, "avenrun_1min");
    if (kn == 0) {
        fprintf(stderr,"not found\n");
        exit(1);
    }
    *loads = kn->value.ul/(FSCALE/100);
    kstat_close(kc);
    return 0;
#else
    /* yes, this isn't right */
    loads[0] = loads[1] = loads[2] = 0;
    return -1;
#endif
} 
