#ifndef NINECC_H
#define NINECC_H

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
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
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_EQUAL, // ==
  ND_NEQUAL, // !=
  ND_GT, // >
  ND_EGT, // =>
  ND_LT, // <
  ND_ELT, // =<
  ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

Token *tokenize();

Node *expr();
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



#endif