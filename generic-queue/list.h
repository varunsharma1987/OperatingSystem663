#ifndef H_LIST
#define H_LIST

#include <stdbool.h>
#include <stddef.h>

struct list_elem
{
    struct list_elem *prev;
    struct list_elem *next;
};

struct list
{
    struct list_elem head;
    struct list_elem tail;
};

/* ####### macros ####### */

/* This will get the entry to the user-defined struct body. */
#define list_entry(LIST_ELEM, STRUCT, MEMBER) \
            ((STRUCT *) ((unsigned char *) &(LIST_ELEM)->next     \
                - offsetof (STRUCT, MEMBER.next)))

/* ###### typedefs ###### */

/* a function pointer. Returns true if a < b, false if
 * a >= b. The argument aux may not be used.
 */
typedef bool 
list_less_func(const struct list_elem *a,
                const struct list_elem *b,
                void *aux);

/* ################################# */
/* ###### function primitives ###### */
/* ################################# */

/* functions for list initialization */
void
list_init(struct list*);

/* functions for list iteration */
struct list_elem
*list_head(struct list*);

struct list_elem
*list_tail(struct list*);

struct list_elem
*list_begin(struct list*);

struct list_elem
*list_end(struct list*);

struct list_elem
*list_next(struct list_elem*);

struct list_elem
*list_prev(struct list_elem*);

/* functions on determining node position
 * (used in assertions of iteration) */
static inline bool
is_head(struct list_elem*);

static inline bool
is_interior(struct list_elem*);

static inline bool
is_tail(struct list_elem*);

bool
is_list_empty(struct list*);

/* functions for list modification */
void
list_insert_head(struct list*, struct list_elem*);

void
list_insert_tail(struct list*, struct list_elem*);

struct list_elem*
list_remove(struct list_elem*);

#endif
