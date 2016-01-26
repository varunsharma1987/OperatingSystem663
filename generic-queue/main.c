#include "list.h"
#include <stdio.h>
#include <stdlib.h>

struct test {
    struct list_elem elem;
    int number;
};

struct list testlist;

int main(void)
{
    list_init(&testlist);
    struct list_elem *e, *etmp;
    struct test *t, *ttmp;
    int i;
    /* insertion test */
    for (i = 0; i < 5; i++) {
        t = (struct test*)malloc(sizeof(struct test));
        t->number = i;
        e = &t->elem;
        list_insert_head(&testlist, e);
    }
	if (is_list_empty(&testlist))
		printf("Error: Testlist is still EMPTY!!!\n");
    /* iteration test */
    for (e = list_begin(&testlist); e != list_end(&testlist); \
            e = list_next(e))
    {
        t = list_entry(e, struct test, elem);
        printf("output of t: %d\n", t->number);
    }

    return 0;
}
