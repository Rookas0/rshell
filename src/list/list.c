#include "list.h"

struct list *create_list(void) {
    struct list *my_list = malloc(sizeof(struct list));
    struct node *dummy = malloc(sizeof(struct node));

    dummy->next = NULL;
    dummy->prev = NULL;
    dummy->is_cp = true;
    dummy->c = '\0';
    my_list->HEAD = dummy;
    my_list->cursor = dummy;
    return my_list;
}

struct node *create_node(char c) {
    struct node *n = malloc(sizeof(struct node));
    n->next = NULL;
    n->prev = NULL;
    n->c = c;
    return n;
}

bool list_move_cursor_left(struct list *lst)
{
    
    struct node *cursor = lst->cursor;
    if(cursor != NULL && cursor->c != '\0') {
        lst->cursor = cursor->prev;
        return true;
    }
    return false;
}

bool list_move_cursor_right(struct list *lst)
{
    
    struct node *cursor = lst->cursor;
    if(cursor != NULL && cursor->next != NULL) {
        lst->cursor = cursor->next;
        return true;
    }
    return false;
}

bool list_insert_after_cursor(struct list *lst, char c)
{
    struct node *cursor = lst->cursor;
    //printable
    //if(cursor->c != '\0') {
    struct node *new_node = create_node(c);
    if(cursor != NULL) {
        new_node->prev = cursor;
        new_node->next = cursor->next;
        cursor->next = new_node;
        lst->cursor = new_node;

        return true;
    }
    return false;

}

bool list_append_char(struct list *lst, char c)
{
    struct node *cursor = lst->cursor;
    while(cursor != NULL && cursor->next != NULL) {
        cursor = cursor->next;
    }

    list_insert_after_cursor(lst, c);
}
bool list_delete_at_cursor(struct list *lst)
{
    struct node *cursor = lst->cursor;
    if(cursor != NULL && cursor->c != '\0') {
        cursor->prev->next = cursor->next;
        if(cursor-> next != NULL) {
            cursor->next->prev = cursor->prev;
        }
        struct node *prev = cursor->prev;
        free(cursor);
        lst->cursor = prev;

        return true;
    }

    return false;
}
void free_list(struct list* l) {
    struct node *cursor = l->HEAD;

    while(cursor->prev != NULL) {
        cursor = cursor->prev;
    }
    struct node *next;
    while(cursor != NULL) {
        next = cursor->next;
        free(cursor);
        cursor = next;
    }
    free(l);
}
