#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// エラー箇所を報告する
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 次のトークンが期待している記号の時には、トークンを一つ読み進める
void expect(char *op) {
  if (token->kind != TK_RESERVED || 
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len)
  ) {
    error_at(token->str, "expected '%c'", op);
  }
  token = token->next;
}

// 次のトークンが数値の場合、トークンを一つ読み進めてその数値を返す
int expect_number() {
  if (token->kind != TK_NUM) {
    error_at(token->str, "expected a number");
  }
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// 次のトークンが期待している記号の時には、トークンを一つ読み進めて、真を返す
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

Token *consume_ident() {
  if (token->kind != TK_IDENT) {
    return NULL;
  }
  return token;
}

bool is_(char *value) {
  if (token->kind != TK_RESERVED ||
      strlen(value) != token->len ||
      memcmp(token->str, value, token->len))
    return false;
  return true;  
}

LVar *find_lvar(Token *tok) {
  for (LVar *var = current_func_token->locals; var; var = var->next) {
    if (var->len == tok->len && !memcmp(tok->str, var->name, var->len)) {
      return var;
    }
  }
  return NULL;
}

Type *connect_deref(Type *type, int new_type, int array_size) {
  Type *new = calloc(1, sizeof(Type));
  if (new_type == PTR) {
    new->ty = PTR;
  } else if (new_type == ARRAY) {
    new->ty = ARRAY;
    new->array_size = array_size;
  }
  new->ptr_to = type;
  return new;
}

// Lvar以下から作成
// int x;
// int *x;
// int *x[];
// int x[];
LVar *generate_lvar(int arg_type) {
  if (token->kind != TK_TYPE) {
    error("有効な型ではありません。");
  }
  Type *type = calloc(1, sizeof(Type));
  type->ty = INT;
  token = token->next;
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
  LVar *lvar = find_lvar(token);
  if (lvar) {
    error("型定義の後に変数名がありません。");
  }
  LVar *return_lvar = calloc(1, sizeof(LVar));
  return_lvar->name = token->str;
  return_lvar->len = token->len;
  return_lvar->is_arg = arg_type;
  
  token = token->next;

  if (is_("[")) {
    expect("[");
    int num = expect_number();
    type = connect_deref(type, ARRAY, num);
    expect("]");
  }
  return_lvar->type = type;

  return return_lvar;
}

bool is_func(char *value) {
  if (token->next->kind != TK_RESERVED ||
      strlen(value) != token->next->len ||
      memcmp(token->next->str, value, token->next->len))
    return false;
  return true;  
}

bool is_func_def() {
  Token *t = token;
  if (t->kind != TK_IDENT) return false;
  t = t->next;
  if (t->kind != TK_RESERVED ||
      strlen("(") != t->len ||
      memcmp(t->str, "(", t->len))
    return false;
  t = t->next;

  while (true) {
    if (!(t->kind != TK_RESERVED ||
        strlen(")") != t->len ||
        memcmp(t->str, ")", t->len))) {
      break;
    }
    t = t->next;
  }
  if (t->next->kind != TK_RESERVED ||
      strlen("{") != t->next->len ||
      memcmp(t->next->str, "{", t->next->len))
    return false;
  return true;
}


bool consume_return() {
    if (token->kind != TK_RETURN ||
      strlen("return") != token->len ||
      memcmp(token->str, "return", token->len))
    return false;
  token = token->next;
  return true;
}

bool is_block_start() {
    if (token->kind != TK_RESERVED ||
      strlen("{") != token->len ||
      memcmp(token->str, "{", token->len))
    return false;
  return true;  
}

bool is_block_end() {
    if (token->kind != TK_RESERVED ||
      strlen("}") != token->len ||
      memcmp(token->str, "}", token->len))
    return false;
  return true;  
}

bool consume_if() {
      if (token->kind != TK_IF ||
      strlen("if") != token->len ||
      memcmp(token->str, "if", token->len))
    return false;
  token = token->next;
  return true;
}

bool has_else() {
  if (token->kind != TK_ELSE || 
      strlen("else") != token->len ||
      memcmp(token->str, "else", token->len)
  ) {
    return false;
  }
  return true;
}

bool consume_while() {
      if (token->kind != TK_WHILE ||
      strlen("while") != token->len ||
      memcmp(token->str, "while", token->len))
    return false;
  token = token->next;
  return true;
}

bool consume_for() {
  if (token->kind != TK_FOR ||
      strlen("for") != token->len ||
      memcmp(token->str, "for", token->len))
    return false;
  token = token->next;
  return true;
}

bool consume_sizeof() {
  if (token->kind != TK_SIZEOF ||
      strlen("sizeof") != token->len ||
      memcmp(token->str, "sizeof", token->len))
    return false;
  token = token->next;
  return true;
}

bool is_type(char *type) {
  if (token->kind != TK_TYPE ||
      strlen(type) != token->len ||
      memcmp(token->str, type, token->len))
    return false;
  return true;
}


// 新しいトークンを作成して、curと連結する
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

bool startswith(char *p, char *q) {
  return memcmp(p, q, strlen(q)) == 0;
}

Token *tokenize() {
  char *p = user_input;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    // Multi-letter punctuator
    if (startswith(p, "==") || startswith(p, "!=") ||
        startswith(p, "<=") || startswith(p, ">=")) {
      cur = new_token(TK_RESERVED, cur, p, 2);
      p += 2;
      continue;
    }

    if (strchr("+-*/()<>=;{},&[]", *p)) {
      cur = new_token(TK_RESERVED, cur, p++, 1);
      continue;
    }

    if (strncmp(p, "return", 6) == 0 && !isalnum(p[6])) {
      cur = new_token(TK_RETURN, cur, p, 6);
      p += 6;
      continue;
    }

    if (strncmp(p, "if", 2) == 0 && !isalnum(p[2])) {
      cur = new_token(TK_IF, cur, p, 2);
      p += 2;
      continue;
    }

    if (strncmp(p, "else", 4) == 0 && !isalnum(p[4])) {
      cur = new_token(TK_ELSE, cur, p, 4);
      p += 4;
      continue;
    }

    if (strncmp(p, "while", 5) == 0 && !isalnum(p[5])) {
      cur = new_token(TK_WHILE, cur, p, 5);
      p += 5;
      continue;
    }

    if (strncmp(p, "for", 3) == 0 && !isalnum(p[3])) {
      cur = new_token(TK_FOR, cur, p, 3);
      p += 3;
      continue;
    }

    if (strncmp(p, "int", 3) == 0 && !isalnum(p[3])) {
      cur = new_token(TK_TYPE, cur, p, 3);
      p += 3;
      continue;
    }

    if (strncmp(p, "sizeof", 6) == 0 && !isalnum(p[6])) {
      cur = new_token(TK_SIZEOF, cur, p, 6);
      p += 6;
      continue;
    }

    char *var;
    if (isalpha(*p)) {
      int length = 0;
      for (;;) {
        if (isalnum(p[length])) {
          length += 1;
        } else {
          break;
        }
      }
      cur = new_token(TK_IDENT, cur, p, 0);
      p += length;
      cur->len = length;
      continue;      
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p, 0);
      char *q = p;
      cur->val = strtol(p, &p, 10);
      cur->len = p - q;
      continue;
    }
    error("トークナイズできません。");
  }

  new_token(TK_EOF, cur, p, 0);
  return head.next;
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs, Type *type) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  node->type = type;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  
  Type *type = calloc(1, sizeof(Type));
  type->ty = INT;
  node->type = type;
  return node;
}

// program = function*
void *program() {
  int i = 0;
  while (!at_eof()) {
    code[i] = function();
    i++;
  }
  code[i] = NULL;
}

// function = ident "(" ")" "{" stmt* "}"
Node *function() {
  if (token->kind != TK_TYPE) {
    error("関数の戻り値の型が定義されていません。");
  }
  token = token->next;
  Node *node = calloc(1, sizeof(Node));
  current_func_token = node;
  node->kind = ND_FUNC_DEF;
  node->name = token->str;
  node->len = token->len;
  node->type = NULL;
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

// stmt = expr ";" 
//       | int ident ";"
//       | int ident "[" unary "]" ";"
//       | "return" expr ";"
//       | "{" stmt* "}"
//       | "if" "(" expr ")" stmt ("else" stmt)?
//       | "while" "(" expr ")" stmt
//       | "for" "(" expr? ";" expr? ";" expr? ")" stmt
Node *stmt() {
  if (consume_return()) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_RETURN;
    node->lhs = expr();
    expect(";");
    return node;
  } else if (is_block_start()) {
    expect("{");
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_BLOCK;
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
  } else if (consume_if()) {
    expect("(");
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_IF;
    node->lhs = expr();
    expect(")");
    Node *true_stmt = stmt();
    
    if (has_else()) {
      token = token->next;
      node->branch[0] = true_stmt;
      node->branch[1] = stmt();
    } else {
      node->branch[0] = true_stmt;
    }
    return node;
  } else if (consume_while()) {
    expect("(");
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_WHILE;
    node->lhs = expr();
    expect(")");
    node->rhs = stmt();
    return node;
  } else if (consume_for()) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_FOR;
    expect("(");
    node->branch[0] = expr();
    expect(";");
    node->branch[1] = expr();
    expect(";");
    node->branch[2] = expr();
    expect(")");
    node->branch[3] = stmt();
    return node;
  } else if (is_type("int")) {
    LVar *lvar = generate_lvar(0);
    if (lvar == NULL) {
      error("有効な変数ではありません。");
    } else {
      add_local_variable(lvar);
      Node *node_lvar_def = calloc(1, sizeof(Node));
      node_lvar_def->kind = ND_LVAR_DEF;
      node_lvar_def->offset = lvar->offset;
      node_lvar_def->locals = lvar;
      expect(";");
      return node_lvar_def;
    }
  } else {
    Node *node = expr();
    expect(";");
    return node;
  }
}

// expr = assign
Node *expr() {
  return assign();
}

// assign = equality ("=" assign)?
Node *assign() {
  Node *node = equality();
  if (consume("=")) {
    Node *a_node = assign();
    Type *type = find_assign_type(node->type, a_node->type);
    node = new_node(ND_ASSIGN, node, a_node, type);
  }
  return node;
}


// equality = relational ("==" relational | "!=" relational)*
Node *equality() {
  Node *node = relational();

  for (;;) {
    if (consume("==")) {
      node = new_node(ND_EQUAL, node, relational(), NULL);
    }
    else if (consume("!=")) {
      node = new_node(ND_NEQUAL, node, relational(), NULL);
    }
    else
      return node;
  }
}

// relational = add ("<" add | "<=" add | ">" add | ">=" add)*
Node *relational() {
  Node *node = add();
  for (;;) {
    if (consume(">"))
      node = new_node(ND_GT, node, add(), NULL);
    else if (consume(">="))
      node = new_node(ND_EGT, node, add(), NULL);
    else if (consume("<"))
      node = new_node(ND_LT, node, add(), NULL);
    else if (consume("<="))
      node = new_node(ND_ELT, node, add(), NULL);  
    else
      return node;
  }
}

// TODO 配列の足し算に対応する
// add = mul ("+" mul | "-" mul)*
Node *add() {
  Node *node = mul();
  for (;;) {
    if (consume("+")) {
      Node *a_unary = mul();
      node = operate_add(node, a_unary);
    } else if (consume("-")) {
      Node *s_unary = mul();
      node = operate_sub(node, s_unary);
    } else {
      return node;
    }
  }
}

// mul = unary ("*" unary | "/" unary)*
Node *mul() {
  Node *node = unary();
  for (;;) {
    if (consume("*")) {
      Node *m_unary = unary();
      if (node->type->ty != INT || m_unary->type->ty != INT) {
        error("掛け算の両辺がINTではありません。");
      }
      node = new_node(ND_MUL, node, m_unary, node->type);
    } else if (consume("/")) {
      Node *d_unary = unary();
      if (node->type->ty != INT || d_unary->type->ty != INT) {
        error("割り算の両辺がINTではありません。");
      }
      node = new_node(ND_DIV, node, d_unary, node->type);
    } else {
      return node;
    }
  }
}

// unary = "+"? primary
//       | "-"? primary
//       | "*" unary
//       | "&" unary
//       | "sizeof" unary
//       | primary
Node *unary() {
  if (consume("+")) {
    return primary();
  }
  if (consume("-")) {
    Node *node = primary();
    return new_node(ND_SUB, new_node_num(0), node, node->type);
  }

  if (is_("*")) {
    token = token->next;
    Node *deref_node = unary();
    Type *new;
    if (deref_node->type->ty != PTR && deref_node->type->ty != ARRAY) {
      error("*演算子が対応していない型です。");
    } else {
      new = deref_node->type->ptr_to;
    }
    return new_node(ND_DEREF, deref_node, NULL, new);
  }

  if (is_("&")) {
    token = token->next;
    Node *addr_node = unary();
    if (addr_node->kind != ND_LVAR) {
      error("&演算子は変数の前のみ記述可能です");
    }

    Type *type = calloc(1, sizeof(Type));
    type->ty = PTR;
    type->ptr_to = addr_node->type;

    return new_node(ND_ADDR, addr_node, NULL, type);
  }

  if (consume_sizeof()) {
    Node *sizeof_unary = unary();
    int size = get_size_of(sizeof_unary->type);
    return new_node_num(size);
  }

  return primary();
}

// primary = num 
//           | ident( "(" ( expr ("," expr)* )? ")" 変数、関数呼び出し
//           | "(" expr ")"
Node *primary() {
  if (consume("(")) {
    Node *node = expr();
    expect(")");
    return node;
  }

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    if(is_func("(")) {
      // function call
      node->kind = ND_FUNC_CALL;
      node->name = tok->str;
      node->len = tok->len;
      token = token->next;
      consume("(");
      int i = 0;
      for (;;) {
        if (is_(")")) {
          break;
        }
        node->branch[i] = expr();
        i++;
        consume(",");
      }
      consume(")");
      node->type = get_function_sig(node);
      return node;
    }
    node->kind = ND_LVAR;

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->offset = lvar->offset;
      node->locals = lvar;
      node->type = lvar->type;
    } else {
      error("未定義の変数を利用しています。");
    }

    token = token->next;
    return node;
  }
  return new_node_num(expect_number());
}