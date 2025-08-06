#pragma once
enum node_type_t {
    COMMAND,
    PIPE,
};

struct token_stream
{
    struct token_list *list;
    int pos;
};
struct command {
    char **argv;
    int argc;
    int capacity;
};

struct ot_node {
    //struct ot_node *parent;
    enum node_type_t type;
    union {
        struct {
            struct ot_node *left_child;
            struct ot_node *right_child;
        } children;
        struct command *cmd;
    };

};

struct operator_tree {
    struct ot_node *root;    
};

struct command* init_cmd(int capac);
struct ot_node* parse_cmd(struct token_stream *ts);
struct ot_node* parse(struct token_list *tl);
void free_operator_tree(struct ot_node *node);
