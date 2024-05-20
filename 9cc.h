#ifndef NINECC_H
#define NINECC_H
#include <stddef.h>


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
  TK_TYPE,     // 型
  TK_SIZEOF,   // sizeof
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
  ND_FUNC_CALL,   // 関数呼び出し　　　　   // 6
  ND_FUNC_DEF,    // 関数定義 // 7
  ND_EQUAL,  // ==           // 8
  ND_NEQUAL, // !=           // 9
  ND_GT,     // >            // 10
  ND_EGT,    // =>           // 11
  ND_LT,     // <            // 12
  ND_ELT,    // =<           // 13
  ND_RETURN, // return       // 14
  ND_IF,     // if           // 15
  ND_WHILE,  // while        // 16
  ND_FOR,    // for          // 17
  ND_BLOCK,  // block        // 18
  ND_NUM,    // 整数          // 19
  ND_ADDR, // &          // 20
  ND_DEREF,   // *          // 21
  ND_LVAR_DEF,   // int         // 22
} NodeKind;


struct Type {
  enum {INT, PTR, ARRAY} ty;
  struct Type *ptr_to;
  size_t array_size;
};

typedef struct Node Node;

typedef struct LVar LVar;

typedef struct Type Type;

// ローカル変数の型
struct LVar {
  LVar *next;
  char *name;
  int len;
  int offset;
  int is_arg; // 0: ローカル変数 1:引数
  Type *type;
};

struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;        // kindがND_NUMの場合のみ使う
  int offset;     // kindがDN_LVARの場合のみ使う
  Node *branch[1000]; // kindがND_IFやforの場合使う(ifはif-elseのみ対応)
  char *name;    // 関数の場合
  int len;       // 関数の場合
  LVar *locals;
  Type *type;
};

typedef struct FunctionSig FunctionSig;

// 関数の型安全を保証するための構造体
// TODO 引数型も確認する
struct FunctionSig {
  FunctionSig *next;
  char *name;
  Type *return_type;
};


// LabelのStack番号管理用
typedef struct {
    int stack[100];
    int top;
    int currentLabel;
} LabelStack;


void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);

Token *tokenize();

void *program();
Node *function();
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
void gen_function_def(Node *node);


LabelStack *create_label_stack();
int push_label(LabelStack *stack);
int pop_label(LabelStack *stack);


// 現在着目しているトークン
extern Token *token;

// 入力プログラム
extern char *user_input;

// 複数行分のNode
extern Node *code[100];

// 関数のsignature情報を入れる
extern FunctionSig *function_sig;

// 現在見ている関数のToken
extern Node *current_func_token;

// ローカル変数
extern LVar *locals;

// Label記憶用のStack
extern LabelStack *labelStackIf;
extern LabelStack *labelStackWhile;
extern LabelStack *labelStackFor;

// parse.c
Node *new_node(NodeKind kind, Node *lhs, Node *rhs, Type *type);
Node *new_node_num(int val);

// util
void add_function_sig(Node *node);
Type *get_function_sig(Node *node);
int get_size_of(Type *type);
void add_local_variable(LVar *lvar);
Type *find_assign_type(Type *l, Type *r);

// operator
int get_add_size(Type *t);
Node *operate_add(Node *l, Node *r);
Node *operate_sub(Node *l, Node *r);

// Debug用関数
void printTokens(Token token);
void printToken(Token token);

#endif