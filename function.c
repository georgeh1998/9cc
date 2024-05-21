#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node *generate_function_def_node() {
    // 関数 TODO voidもサポートする
    Node *node = calloc(1, sizeof(Node));
    current_func_token = node;
    node->kind = ND_FUNC_DEF;
    node->name = token->str;
    node->len = token->len;
    Type *f_type = calloc(1, sizeof(Type));
    f_type->ty = INT;
    node->type = f_type;
    token = token->next;
    expect("(");
    for (;;) {
    if (is_(")")) {
        break;
    }
    if (!is_type("int")) {
        error("関数の引数に型の定義がありません。");
    }
    LVar *lvar = generate_lvar(1);
    if (lvar) {
        add_local_variable(lvar);
    } else {
        error("有効な変数ではありません。");
    }
    if (is_(")")) {
        break;
    }
    token = token->next;
    consume(",");
    }
    expect(")");
    add_function_sig(node);
    expect("{");
    int i = 0;
    for (;;) {
    node->branch[i] = stmt();
    i += 1;
    if (is_block_end()) {
        break;
    }
    }
    expect("}");
    return node;
}