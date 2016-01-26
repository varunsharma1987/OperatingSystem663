#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "list.h"

void
list_init(struct list *list)
{
    assert(list);
    list->head.prev = NULL;
    list->head.next = &list->tail;
    list->tail.prev = &list->head;
    list->tail.next = NULL;
    assert(is_list_empty(list));
}

struct list_elem
*list_head(struct list* list)
{
    assert(list);
    return &list->head;
}

struct list_elem
*list_tail(struct list* list)
{
    assert(list);
    return &list->tail;
}

struct list_elem
*list_begin(struct list* list)
{
    assert(list);
    return list->head.next;
}

struct list_elem
*list_end(struct list* list)
{
    assert(list);
    return list->tail.prev;
}

struct list_elem
*list_next(struct list_elem *e)
{
    assert(e);
    assert(is_head(e) || is_interior(e));
    return e->next;
}

struct list_elem
*list_prev(struct list_elem *e)
{
    assert(e);
    assert(is_interior(e) || is_tail(e));
    return e->prev;
}

bool
is_list_empty(struct list *list)
{
    assert(list);
    return (list_begin(list) == list_tail(list) \
            && list_end(list) == list_head(list));
}

inline bool
is_head(struct list_elem *e)
{
    assert (e);
    return (e->prev == NULL && e->next != NULL);
}

inline bool
is_interior(struct list_elem *e)
{
    assert (e);
    return (e->prev != NULL && e->next != NULL);
}

inline bool
is_tail(struct list_elem *e)
{
    assert (e);
    return (e->prev != NULL && e->next == NULL);
}

void
list_insert_head(struct list *list, struct list_elem *e)
{
    assert(list);
    assert(e);
    struct list_elem *then_first = list->head.next;
    list->head.next = e;
    then_first->prev = e;
    e->next = then_first;
    e->prev = &list->head;
}

void
list_insert_tail(struct list *list, struct list_elem *e)
{
    assert(list);
    assert(e);
    struct list_elem *then_last = list->tail.prev;
    list->tail.prev = e;
    then_last->next = e;
    e->next = &list->tail;
    e->prev = then_last;
}

struct list_elem*
list_remove(struct list_elem *e)
{
    assert(e);
    assert(is_interior(e));
    e->prev->next = e->next;
    e->next->prev = e->prev;
    return e->next;
}
