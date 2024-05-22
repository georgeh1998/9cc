#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// グローバル変数
GVar *gvar;

GVar *find_gvar(Token *tok) {
  for (GVar *var = gvar; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
}

void add_global_variable(GVar *add_g) {

    if (gvar == NULL) {
        gvar = add_g;
        return;
    }

    for (GVar *g = gvar; g; g = g->next) {
        if (g->next == NULL) {
            g->next = add_g;
            return;
        }
    }
}


// Gvarを以下から作成
// int x;
// int *x;
// int *x[];
// int x[];
Node *generate_gvar_node(Type *type) {
  
  for (;;) {
    if (is_("*")) {
      type = connect_deref(type, PTR, 0);
      consume("*");
    } else {
      break;
    }
  }

  if (token->kind != TK_IDENT) {
    error("変数名が定義されていません。");
  }
  GVar *gvar = calloc(1, sizeof(GVar));
  gvar->name = token->str;
  gvar->len = token->len;
  token = token->next;

  if (is_("[")) {
    expect("[");
    int num = expect_number();
    type = connect_deref(type, ARRAY, num);
    expect("]");
  }
  gvar->type = type;
  add_global_variable(gvar);

  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_GVAR_DEF;
  node->type = type;
  expect(";");

  return node;
}