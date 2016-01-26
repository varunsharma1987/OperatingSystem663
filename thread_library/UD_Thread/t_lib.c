#include "t_lib.h"
#include <assert.h>

struct list q_running;
struct list q_ready_H;
struct list q_ready_L;

void t_yield()
{
    ucontext_t *curr;
    ucontext_t *next;
    tcb *tcbnext;
    assert(!is_list_empty(&q_running));

    /* Move the running thread to tail of ready queue */
    tcb *tmpthread = (tcb*)calloc(1, sizeof(tcb));
    getcontext(&tmpthread->context);
    tcb *running = list_entry(list_begin(&q_running), tcb, elem);
    tmpthread->thread_id = running->thread_id;
    tmpthread->priority = running->priority;
    int queueflag = -1;

    if (tmpthread->priority == 0) {
        assert(running->priority == 0);
        list_insert_tail(&q_ready_H, &tmpthread->elem);
        curr = &list_entry(list_end(&q_ready_H), tcb, elem)->context;
    }
    else if (tmpthread->priority == 1) {
        list_insert_tail(&q_ready_L, &tmpthread->elem);
        curr = &list_entry(list_end(&q_ready_L), tcb, elem)->context;
    }
    list_remove(list_begin(&q_running));
    /* Move the 1st thread of the ready queue into running */
    if (!is_list_empty(&q_ready_H)) {
        tcbnext = list_entry(list_begin(&q_ready_H), tcb, elem);
        queueflag = 0;
    }
    else if (!is_list_empty(&q_ready_L)) {
        tcbnext = list_entry(list_begin(&q_ready_L), tcb, elem);
        queueflag = 1;
    }
    assert(queueflag != -1);

    tcb *tmpthread1 = (tcb*)calloc(1, sizeof(tcb));
    tmpthread1->context = tcbnext->context;
    tmpthread1->thread_id = tcbnext->thread_id;
    tmpthread1->priority = tcbnext->priority;
    
    list_insert_head(&q_running, &tmpthread1->elem);
    next = &list_entry(list_begin(&q_running), tcb, elem)->context;
    if (queueflag == 0)
        list_remove(list_begin(&q_ready_H));
    else if (queueflag == 1)
        list_remove(list_begin(&q_ready_L));

    swapcontext(curr, next);
}

void t_init()
{
    list_init(&q_running);
    list_init(&q_ready_H);
    list_init(&q_ready_L);

    tcb *tmpthread;
    tmpthread = (tcb*)calloc(1, sizeof(tcb));
    /* initializing the main thread */
    getcontext(&tmpthread->context);
    tmpthread->priority = 1;
    tmpthread->thread_id = -1;
    list_insert_head(&q_running, &tmpthread->elem);
}

void
t_terminate()
{
    ucontext_t *next;
    int queueflag = 0;
    if (!is_list_empty(&q_ready_H)) {
        queueflag = 0;
        next = &list_entry(list_begin(&q_ready_H), tcb, elem)->context;
    }
    else if (!is_list_empty(&q_ready_L)) {
        queueflag = 1;
        next = &list_entry(list_begin(&q_ready_L), tcb, elem)->context;
    }
    struct list_elem *e = list_begin(&q_running);
    list_remove(e);
    free(list_entry(e, tcb, elem));
    tcb *tmpthread = (tcb*)calloc(1, sizeof(tcb));
    /* problem: why must I build a new struct tmpthread, and 
     * insert(tmpthread->elem)? why can't I just do
     * insert(list_begin(&q_ready))  ????
     */
    if (queueflag == 0)
        *tmpthread = *list_entry(list_begin(&q_ready_H), tcb, elem);
    else
        *tmpthread = *list_entry(list_begin(&q_ready_L), tcb, elem);
        
    list_insert_head(&q_running, &tmpthread->elem);
    if (queueflag == 0)
        list_remove(list_begin(&q_ready_H));
    else
        list_remove(list_begin(&q_ready_L));

    setcontext(next);
}

void
t_shutdown()
{
    struct list_elem *e;
    tcb *tmp;
    while(!is_list_empty(&q_ready_H)) {
        tmp = list_entry(list_begin(&q_ready_H), tcb, elem);
        e = list_begin(&q_ready_H);
        list_remove(e);
        free(tmp);
    }
    while(!is_list_empty(&q_ready_L)) {
        tmp = list_entry(list_begin(&q_ready_L), tcb, elem);
        e = list_begin(&q_ready_L);
        list_remove(e);
        free(tmp);
    }
    while(!is_list_empty(&q_running)) {
        tmp = list_entry(list_begin(&q_running), tcb, elem);
        e = list_begin(&q_running);
        list_remove(e);
        free(tmp);
    }
}

int t_create(void (*fct)(int), int id, int pri)
{
    size_t sz = 0x10000;

    ucontext_t *uc;
    uc = (ucontext_t *)calloc(1, sizeof(ucontext_t));

    getcontext(uc);
    uc->uc_stack.ss_sp = calloc(1, sz);  
    uc->uc_stack.ss_size = sz;
    uc->uc_stack.ss_flags = 0;
    uc->uc_link = 0;
    makecontext(uc, fct, 1, id);

    tcb *tmpthread = (tcb*)calloc(1, sizeof(tcb));
    tmpthread->context = *uc;
    tmpthread->thread_id = id;
    tmpthread->priority = pri;
    if (pri == 0) {
        list_insert_tail(&q_ready_H, &tmpthread->elem);
    }
    else if (pri == 1)
        list_insert_tail(&q_ready_L, &tmpthread->elem);
}
