#include "list.h"

struct list *create_list(char c) {
    struct list *my_list = malloc(sizeof(struct list));

    my_list->next = NULL;
    my_list->prev = NULL;
    my_list->is_cp = true;
    my_list->c = c;

    return my_list;
}

void free_list(struct list* l) {
    while(l->prev != NULL) {
        l = l->prev;
    }
    struct list *next;
    while(l != NULL) {
        next = l->next;
        free(l);
        l = next;
    }
}
