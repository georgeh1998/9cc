#ifndef NINECC_H
#define NINECC_H

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_RETURN,   // return
  TK_IF,       // if
  TK_NUM,      // 整数トークン
  TK_EOF,      // 入力の終わりを表すトークン
} TokenKind;

typedef struct Token Token;

// トークン型
struct Token {
  TokenKind kind;
  Token *next;
  int val;
  char *str;
  int len;
};


// 抽象構文木のノードの種類
typedef enum {
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_ASSIGN, // =
  ND_LVAR,   // ローカル変数
  ND_EQUAL,  // ==
  ND_NEQUAL, // !=
  ND_GT,     // >
  ND_EGT,    // =>
  ND_LT,     // <
  ND_ELT,    // =<
  ND_RETURN, // return
  ND_IF,    // if
  ND_NUM,    // 整数
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;    // kindがND_NUMの場合のみ使う
  int offset; // kindがDN_LVARの場合のみ使う
};

typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
};


void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

Token *tokenize();

void *program();
Node *stmt();
Node *expr();
Node *assign();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();  
Node *primary();


void gen(Node *node);


// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern char *user_input;

// 複数行分のNode
extern Node *code[100];

// ローカル変数
extern LVar *locals;

// label
extern 


// Debug用関数
void printTokens(Token token);
void printToken(Token token);

#endif