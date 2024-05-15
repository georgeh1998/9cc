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