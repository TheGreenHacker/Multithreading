#include "SortedList.h"
#include <sched.h>
#include <stdlib.h>
#include <string.h>

void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
    SortedListElement_t *p = list->next;
    while (p != list && strcmp(p->key, element->key) <= 0)
        p = p->next;
    if (opt_yield & INSERT_YIELD)
        sched_yield();
    element->prev = p->prev;
    p->prev->next = element;
    element->next = p;
    p->prev = element;
}

int SortedList_delete(SortedListElement_t *element)
{
    if (element->prev->next == element && element->next->prev == element){
        if (opt_yield & DELETE_YIELD)
            sched_yield();
        element->prev->next = element->next;
        element->next->prev = element->prev;
        return 0;
    }
    return 1;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key)
{
    SortedListElement_t *p = list->next;
    while (p != list && strcmp(p->key, key)){
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        p = p->next;
    }
    return p;
}

int SortedList_length(SortedList_t *list)
{
    int length = 0;
    SortedList_t *p = list->next;
    while (p != list){
        length++;
        if (opt_yield & LOOKUP_YIELD)
            sched_yield();
        p = p->next;
    }
    return length;
}
