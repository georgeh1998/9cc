#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int get_add_size(Type *t) {
    if (t->ty == CHAR) {
        return 1;
    }
    if (t->ty == INT) {
        return 1;
    }
    if (t->ty == PTR) {
        if (t->ptr_to->ty == INT) {
            return +4;
        } else if (t->ptr_to->ty = PTR) {
            return +8;
        } else {
            error("ポインタの先がサポートしていない型です");
        }
    }
    if (t->ty == ARRAY) {
        if (t->ptr_to->ty == CHAR) {
            return +1;
        } else if (t->ptr_to->ty == INT) {
            return +4;
        } else if (t->ptr_to->ty == PTR) {
            return +8;
        } else {
            error("配列の中身がサポートしていない型です");
        }
    }
    error("unknown error at get_add_size");
}

/**
 * int x[]; x + 1;
 * int a; int x[]; *(x+a);
*/
Node *operate_add(Node *l, Node *r) {
    Type *lt = l->type;
    Type *rt = r->type;

    if (lt->ty == INT && rt->ty == INT) {
        return new_node(ND_ADD, l, r, rt);
    }

    if (lt->ty == CHAR && rt->ty == CHAR) {
        return new_node(ND_ADD, l, r, rt);
    }

    if (lt->ty == INT) {
        Node *m_node = new_node(ND_MUL, new_node_num(get_add_size(rt)), l, lt);
        return new_node(ND_ADD, r, m_node, rt);
    } else if (rt->ty == INT) {
        Node *m_node = new_node(ND_MUL, new_node_num(get_add_size(lt)), r, rt);
        return new_node(ND_ADD, l, m_node, lt);
    } else {
        error("加算の両オペランドが想定していない型です。");
    }
}

Node *operate_sub(Node *l, Node *r) {
    Type *lt = l->type;
    Type *rt = r->type;

    if (lt->ty != INT || rt->ty != INT) {
        error("減算のオペランドが想定していない型です。");
    }
    return new_node(ND_SUB, l, r, rt);
}
