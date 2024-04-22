#ifndef NINECC_H
#define NINECC_H

// トークンの種類
typedef enum {
  TK_RESERVED, // 記号
  TK_IDENT,    // 識別子
  TK_RETURN,   // return
  TK_IF,       // if
  TK_ELSE,     // else
  TK_WHILE,    // while
  TK_FOR,      // for
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
  ND_ADD,    // +            // 0
  ND_SUB,    // -            // 1
  ND_MUL,    // *            // 2
  ND_DIV,    // /            // 3
  ND_ASSIGN, // =            // 4
  ND_LVAR,   // ローカル変数   // 5
  ND_EQUAL,  // ==           // 6
  ND_NEQUAL, // !=           // 7
  ND_GT,     // >            // 8
  ND_EGT,    // =>           // 9
  ND_LT,     // <            // 10
  ND_ELT,    // =<           // 11
  ND_RETURN, // return       // 12
  ND_IF,     // if           // 13
  ND_WHILE,  // while        // 14
  ND_FOR,    // for          // 16
  ND_NUM,    // 整数          // 17
} NodeKind;

typedef struct Node Node;

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;        // kindがND_NUMの場合のみ使う
  int offset;     // kindがDN_LVARの場合のみ使う
  Node *branch[4]; // kindがND_IFやforの場合使う(ifはif-elseのみ対応)
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