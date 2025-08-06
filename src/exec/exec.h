#pragma once
#include "../tokenize/tokenize.h"
#include "../parser/parser.h"

int exec_cmd(struct command *tl);
void exec_tree(struct ot_node *node);
int mpipe(struct token_list *tl);
void exec_exit(struct token_list *tl);
