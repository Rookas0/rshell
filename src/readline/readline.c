#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../list/list.h"
#include "./readline.h"
 
struct line line_info;

static void init_string(struct string *str) {
    str->capac = 128;
    str->size = 0;
    str->s = malloc(sizeof(char) * (str->capac + 1));
    if(!str->s) {
        perror("malloc");
    }
}

static void append_char(struct string *str, char c) {
    if(str->size >= str->capac) {
        str->capac *= 2;
        char *tmp = realloc(str->s, sizeof(char) * (str->capac + 1));
        if(!tmp) {
            perror("realloc");
            return;
        }
    }
    str->s[str->size++] = c;
    str->s[str->size] = '\0';
}

static void insert_char(struct line *ln, char c) {
    struct string *str = &ln->str;
    int pos = ln->posx;

    if(str->size >= str->capac) {
        str->capac *= 2;
        char *tmp = realloc(str->s, sizeof(char) * (str->capac + 1));
        if(!tmp) {
            perror("realloc");
            return;
        }
    }
    
    if(str->size == ln->posx) {
        str->s[pos] = c;
        str->s[str->size++] = c;
        ln->posx++;
        return;
    }
    //shift
    for(int i = str->size - 1; i > pos; i--) {
        str->s[i+1] = str->s[i];
    }

    str->s[pos] = c;
    str->s[str->size++] = c;
    // 
}

static void print_prompt(struct line ln, char * prompt) {
    if(prompt == NULL) {
        prompt = "> ";
    }

    write(STDOUT_FILENO, "\r\x1b[?25l", 7);

    write(STDOUT_FILENO, prompt, strlen(prompt));

    write(STDOUT_FILENO, ln.str.s, strlen(ln.str.s));

    char buf[16];
    if(line_info.posx != 0) {
        sprintf(buf, "\r\x1b[%dC", line_info.posx + (int) strlen(prompt));
        write(STDOUT_FILENO, buf, strlen(buf));
    } else {
        sprintf(buf, "\r\x1b[%dC", (int) strlen(prompt));
        write(STDOUT_FILENO, buf, strlen(buf));
    }

    write(STDOUT_FILENO, "\x1b[?25h", 6);
}

int readchar(void) {
    // referenced kilo text editor for handling escape sequence
    char c;
    read(STDIN_FILENO, &c, 1);

    // if escape sequence isn't followed by anything, escape key was pressed, so return escape
    //  otherwise, decode escape sequence into the proper key
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
    else if(c == 0x03) {
        raise(SIGINT);
        return c;
    }
    return c;
}

void insert_char_at_cursor(struct list *lst, char c)
{
    if(list_insert_after_cursor(lst, c)) {
        line_info.posx++;
    }
}

void move_cursor_left(struct list *lst)
{
    if(list_move_cursor_left(lst)) {
        line_info.posx--;
    }
}

void move_cursor_right(struct list *lst)
{
    if(list_move_cursor_right(lst)) {
        line_info.posx++;
    }
}

void delete_at_cursor(struct list *lst) 
{
    if(list_delete_at_cursor(lst)) {
        line_info.posx--;
    }
}

void append_enter(struct list *lst) {
    char c = '\r';
    list_append_char(lst, c);
}

void handle_char(struct line *ln, int nc)
{
    if(nc >= 0x20 && nc <= 0x7E) {
        char c = (char) nc;
        insert_char(ln, c);
    }
    /*
    if(nc == ARROW_LEFT) {
        move_cursor_left(lst);
    }
    if(nc == ARROW_RIGHT) {
        move_cursor_right(lst);
    }
    if(nc == 0x7F) {
        delete_at_cursor(lst);
    }
    if(nc == 0x0A) {
        append_enter(lst);
    }
    */
}

struct list * readline(char *prompt) {
    char c = '\0';
    struct list *lst = create_list();
    struct line ln;
    ln.posx = 0;
    init_string(&ln.str); 
    line_info.posx = 0;
    for(;;) {
        print_prompt(ln, prompt);
        fflush(stdout);
        //read(STDIN_FILENO, &c, 1);
        int nc = readchar();
        handle_char(&ln, nc);
        // clear line and rewrite
        write(STDOUT_FILENO, "\r\x1b[K", 4);
        //struct list *tmp = head;
        c = (char) nc;

        if(c == '\r' || c == '\n') {
            append_enter(lst);
            //buf[i] = '\0';
            break;
        }
    }
    print_prompt(ln, prompt);
    write(STDOUT_FILENO, "\r\n", 2);
    line_info.posx = 0;
    
    /*
    while(cursor->next != NULL) {
        write(STDOUT_FILENO, &cursor->c, 1);
        cursor = cursor->next;
    } 
    */
    return lst;
}
