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
void error_at(char *loc, char *msg) {
  char *line = loc;
  while (file < line && line[-1] != '\n') {
    line--;
  }
  char *end = loc;
  while (*end != '\n') {
    end++;
  }

  int line_num = 1;
  for (char *p = file; p < line; p++) {
    if (*p == '\n') {
      line_num++;
    }
  }

  int indent = fprintf(stderr, "%s:%d", filename, line_num);
  fprintf(stderr, "%.*s\n", (int)(end - line), line);

  int pos = loc - line + indent;
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ %s\n", msg);
  exit(1);
}

// 次のトークンが期待している記号の時には、トークンを一つ読み進める
void expect(char *op) {
  if (token->kind != TK_RESERVED || 
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len)
  ) {
    char *expected;
    expected = calloc(1, 20 * sizeof(char));
    sprintf(expected, "expected %s", op);
    error_at(token->str, expected);
    
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
  Type *type = get_base_type();
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

bool next_is(char *value) {
  if (token->next->kind != TK_RESERVED ||
      strlen(value) != token->next->len ||
      memcmp(token->next->str, value, token->next->len))
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
  char *p = file;
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    // 行コメント
    if (strncmp(p, "//", 2) == 0) {
      p += 2;
      while (*p != '\n') {
        p++;
      }
      continue;
    }

    // ブロックコメントをスキップ
    if (strncmp(p, "/*", 2) == 0) {
      char *q = strstr(p + 2, "*/");
      if (!q) {
        error_at(p, "コメントが閉じられていません");
      }
      p = q + 2;
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

    if (strncmp(p, "char", 4) == 0 && !isalnum(p[4])) {
      cur = new_token(TK_TYPE, cur, p, 4);
      p += 4;
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

    // 文字列
    if (strncmp(p, "\"", 1) == 0) {
      p += 1;
      int length = 0;
      cur = new_token(TK_STRING, cur, p, 0);
      for (;;) {
        if (p[length] == '"') {
          break;
        } else {
          length += 1;
        }
      }
      p += length + 1;
      cur->len = length;
      continue;
    }
    
    // 変数/関数の名前
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

// function = int ident "(" ")" "{" stmt* "}" // 関数
//          = int ident; // グローバル変数
Node *function() {
  if (token->kind != TK_TYPE) {
    error("関数の戻り値の型が定義されていません。");
  }
  Type *base_type = get_base_type();
  token = token->next;
  if (next_is("(")) {
    return generate_function_def_node(base_type);
  } else {
    return generate_gvar_node(base_type);
  }
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
  } else if (is_type("char")) {
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
    if (type->ty == PTR && type->ptr_to->ty == CHAR) {
      for (int i = 0; i < 100; i++) {
        if (chars[i] == NULL) {
          chars[i] = (char *)malloc(a_node->len + 1);
          strncpy(chars[i], a_node->name, a_node->len);
          chars[i][a_node->len] = '\0';
          a_node->val = i;
          break;
        }  
      }
    }
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
    if (deref_node->type->ty == PTR) {
      new = deref_node->type->ptr_to;
    } else if (deref_node->type->ty == ARRAY) {
      new = deref_node->type->ptr_to;
    } else {
      error("*演算子が対応していない型です。");
    }
    return new_node(ND_DEREF, deref_node, NULL, new);
  }

  if (is_("&")) {
    token = token->next;
    Node *addr_node = unary();
    if (addr_node->kind != ND_LVAR && addr_node->kind != ND_GVAR) {
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
//           | String
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
    if(next_is("(")) {
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
      Type *type = get_function_sig(node);
      node->type = type;
      return node;
    }

    LVar *lvar = find_lvar(tok);
    if (lvar) {
      node->kind = ND_LVAR;
      node->offset = lvar->offset;
      node->locals = lvar;
      node->name = lvar->name;
      node->len = lvar->len;
      node->type = lvar->type;
    } else {
      GVar *gvar = find_gvar(tok);
      if (gvar) {
        node->kind = ND_GVAR;
        node->name = gvar->name;
        node->len = gvar->len;
        node->type = gvar->type;
      } else {
        error("未定義の変数を利用しています。");  
      }
    }
    token = token->next;

    if (is_("[")) {
      expect("[");
      Node *index_node = equality();
      expect("]");
      Node *add_node = operate_add(node, index_node);
      return new_node(ND_DEREF, add_node, NULL, add_node->type->ptr_to);
    }
    return node;
  }

  if (token->kind == TK_STRING) {
    Type *char_type = calloc(1, sizeof(Type));
    char_type->ty = CHAR;
    Type *string_type = calloc(1, sizeof(Type));
    string_type->ty = PTR;
    string_type->ptr_to = char_type;
    Node *string_node = calloc(1, sizeof(Node));
    string_node->kind = ND_STRING;
    string_node->type = string_type;
    string_node->name = token->str;
    string_node->len = token->len;
    token = token->next;
    return string_node;
  }

  return new_node_num(expect_number());
}