/*
 * types used by thread library
 */
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/mman.h>
#include "list.h"

struct tcb {
    int thread_id;
    int priority;
    ucontext_t context;
    struct list_elem elem;
};

typedef struct tcb tcb;
