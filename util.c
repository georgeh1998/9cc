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
    error("関数が定義されていません。");
}

int get_size_of(Type *type) {
    if (type->ty == INT) {
        return 4;
    } else if (type->ty == ARRAY) {
        int each_size = get_size_of(type->ptr_to);
        return each_size * type->array_size;
    } else {
        return 8;
    }
}

LVar *add_local_variable(LVar *lvar) {
    if (current_func_token->locals) {
        for (LVar *var = current_func_token->locals; var; var = var->next) {
            if (var->next == NULL) {
                var->next = lvar;
                lvar->offset = get_size_of(lvar->type);
                break;
            }
        }
    } else {
        lvar->offset = get_size_of(lvar->type);
        current_func_token->locals = lvar;
    }
}