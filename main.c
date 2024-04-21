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
  // 変数26個分の領域を確保する
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  
  int local_var_stack = 0;
  for (LVar *var = locals; var; var = var->next) {
    local_var_stack += 8;
  }
  printf("  sub rsp, %d\n", local_var_stack);
  
  // 抽象構文木を下りながらコード生成
  for (int i = 0; code[i]; i++) {
      gen(code[i]);
  }

  printf("  pop rax\n");  

  // エピローグ
  // 最後の式の結果がRAXに残っているはずなので、それが返り値となる
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}