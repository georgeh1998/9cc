#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void add_function_sig(Node *node) {

    Type *type = calloc(1, sizeof(Type));
    type->ty = INT;

    FunctionSig *fs = calloc(1, sizeof(FunctionSig));
    fs->return_type = type;
    fs->name = node->name;

    if (function_sig == NULL) {
        function_sig = fs;
        return;
    }

    
    for (FunctionSig *f = function_sig; f; f = f->next) {
        if (f->next == NULL) {
            f->next = fs;
            return;
        }
    }
}

Type *get_function_sig(Node *node) {

    for (FunctionSig *f = function_sig; f; f = f->next) {
        if (strncmp(f->name, node->name, node->len) == 0) {
            return f->return_type;
        }
    }
    return NULL;
    error("関数が定義されていません。");
}

int get_size_of(Type *type) {
    if (type->ty == CHAR) {
        return 1;
    } else if (type->ty == INT) {
        return 4;
    } else if (type->ty == ARRAY) {
        int each_size = get_size_of(type->ptr_to);
        return each_size * type->array_size;
    } else {
        return 8;
    }
}

void add_local_variable(LVar *lvar) {
    if (current_func_token->locals) {
        for (LVar *var = current_func_token->locals; var; var = var->next) {
            if (var->next == NULL) {
                lvar->offset = var->offset + get_size_of(lvar->type);
                var->next = lvar;
                break;
            }
        }
    } else {
        lvar->offset = get_size_of(lvar->type);
        current_func_token->locals = lvar;
    }
}


Type *find_assign_type(Type *l, Type *r) {

    if (l->ty == CHAR) {
        if (r->ty == CHAR) {
            return l;
        } else if (r->ty == INT) {
            return r;
        } else {
            error("代入に対応していません: 1");
        }
    }

    if (l->ty == INT) {
        if (r->ty == INT) {
            return l;
        } else if (r->ty == CHAR) {
            return l;
        } else {
            error("代入に対応していません: 2");
        }
    }

    if (l->ty == PTR) {
        if (r->ty == PTR) {
            return l;
        } else if (r->ty == ARRAY) {
            return l;
        } else {
            error("代入に対応していません: 3");
        }
    }

    error("代入に対応していません: 3");
}