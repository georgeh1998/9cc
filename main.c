#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// 複数行分のNode
Node *code[100];

// ローカル変数
LVar *locals;

// Label記憶用のStack
LabelStack *labelStackIf;
LabelStack *labelStackWhile;
LabelStack *labelStackFor;

// 関数を最後に出力するため
Node *functions[100];

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // トークナイズする
  user_input = argv[1];
  token = tokenize();

  // 正しくトークナイズできているかどうかデバッグ用
  // printTokens(*token);

  program();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  // プロローグ
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  
  int local_var_stack = 0;
  for (LVar *var = locals; var; var = var->next) {
    local_var_stack += 8;
  }
  printf("  sub rsp, %d\n", local_var_stack);

  labelStackIf = create_label_stack();
  labelStackWhile = create_label_stack();
  labelStackFor = create_label_stack();
  
  // 抽象構文木を下りながらコード生成
  for (int i = 0; code[i]; i++) {
      gen(code[i]);
  }
  printf("  pop rax\n"); 

  // エピローグ
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");

  // 関数のコード生成
  for (int jj = 0; functions[jj]; jj++) {
    gen_function_def(functions[jj]);
  } 
  return 0;
}