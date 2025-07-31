#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../list/list.h"
#include "./readline.h"
 
struct line_info line_info;

void print_prompt(struct list *head, char * prompt) {
    if(prompt == NULL) {
        prompt = "> ";
    }
    write(STDOUT_FILENO, "\r", 1);
    write(STDOUT_FILENO, prompt, strlen(prompt));
    while(head != NULL) {
        fflush(stdout);
        write(STDOUT_FILENO, &head->c, 1);
        head = head->next;
    }
    char buf[16];
    if(line_info.posx != 0) {
        sprintf(buf, "\r\x1b[%dC", line_info.posx + (int) strlen(prompt));
        write(STDOUT_FILENO, buf, strlen(buf));
    } else {
        sprintf(buf, "\r\x1b[%dC", (int) strlen(prompt));
        write(STDOUT_FILENO, buf, strlen(buf));
    }

}
int readchar(void) {
    // referenced kilo text editor for handling escape sequence
    char c;
    read(STDIN_FILENO, &c, 1);

    // if escape sequence isn't followed by anything, escape key was pressed, so return escape
    if(c == '\x1b') {
        char seq[3];
        if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
        if(seq[0] == '[') {
            if(seq[1] >= '0' && seq[1] <= '9') {
                if(read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if(seq[2] == '~') {
                    switch(seq[1]) {
                        case '1': return HOME_KEY;
                        case '3': return DEL_KEY;
                        case '4': return END_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        case '7': return HOME_KEY;
                        case '8': return END_KEY;
                    }
                }
            } else {
                switch(seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        } else if(seq[0] == 0) {
            switch(seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
    return '\x1b';
    }
    return c;
}

void insert_char_at_cursor(struct list **cursor_p, char c)
{
    
    struct list *cursor = *cursor_p;
    line_info.posx++;
    //printable
    //if(cursor->c != '\0') {
        struct list *new_list = create_list(c);
        new_list->prev = cursor;
        new_list->next = cursor->next;
        cursor->next = new_list;
        *cursor_p = new_list;
    //}
    /*
    else {
        cursor->c = c;
    }
    */
    write(STDOUT_FILENO, &((*cursor_p)->c), 1);
}

void move_cursor_left(struct list **cursor_p)
{
    struct list *cursor = *cursor_p;
    if(cursor != NULL && cursor->c != '\0') {
        *cursor_p = cursor->prev;
        line_info.posx--;
        //write(STDOUT_FILENO, "\x1b[1D", 4);
    }
}

void move_cursor_right(struct list **cursor_p)
{
    struct list *cursor = *cursor_p;
    if(cursor->next != NULL) {
        *cursor_p = cursor->next;
        line_info.posx++;
        //write(STDOUT_FILENO, "\x1b[1D", 4);
    }
}

void delete_at_cursor(struct list **cursor_p) {
    struct list *cursor = *cursor_p;
    if(cursor->c != '\0') {
        cursor->prev->next = cursor->next;
        if(cursor-> next != NULL) {
            cursor->next->prev = cursor->prev;
        }
        free(*cursor_p);
        *cursor_p = cursor;
    }
    move_cursor_left(cursor_p);
}
void handle_char(struct list **cursor_p, int nc)
{
    //struct list *cursor = *cursor_p;
    // sruct list *lp = malloc 
    //  TODO: refactor to make list start at a null char node. 
    //        this will allow cursor to be at start of line before any other nodes.
    //        when printing, skip when node->c == '\0'
    //        also implement right arrow and backspace 
    if(nc >= 0x20 && nc <= 0x7E) {
        char c = (char) nc;
        insert_char_at_cursor(cursor_p, c);
    }
    if(nc == ARROW_LEFT) {
        move_cursor_left(cursor_p);
    }
    if(nc == ARROW_RIGHT) {
        move_cursor_right(cursor_p);
    }
    if(nc == 0x7F) {
        delete_at_cursor(cursor_p);
    }

}
struct list * readline(char *prompt) {
    char c = '\0';
    struct list *cursor = create_list('\0');
    struct list *head = cursor;
    line_info.posx = 0;
    for(;;) {
        print_prompt(head, prompt);
        fflush(stdout);
        //read(STDIN_FILENO, &c, 1);
        int nc = readchar();
        handle_char(&cursor, nc);
        // clear line and rewrite
        write(STDOUT_FILENO, "\r\x1b[K", 4);
        //struct list *tmp = head;
        c = (char) nc;

        if(c == '\r' || c == '\n') {
            //buf[i] = '\0';
            break;
        }
    }
    print_prompt(head, prompt);
    write(STDOUT_FILENO, "\r\n", 2);
    line_info.posx = 0;
    
    /*
    while(cursor->next != NULL) {
        write(STDOUT_FILENO, &cursor->c, 1);
        cursor = cursor->next;
    } 
    */
    return head;
}
