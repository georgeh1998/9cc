#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// 現在着目しているトークン
Token *token;

// 入力プログラム
char *user_input;

// 複数行分のNode
Node *code[100];

// 関数のsignature情報を入れる
FunctionSig *function_sig;

// 現在見ている関数のToken
Node *current_func_token;

// Label記憶用のStack
LabelStack *labelStackIf;
LabelStack *labelStackWhile;
LabelStack *labelStackFor;


int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "引数の個数が正しくありません\n");
    return 1;
  }

  // トークナイズする
  user_input = argv[1];
  token = tokenize();

  program();

  // アセンブリの前半部分を出力
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");

  labelStackIf = create_label_stack();
  labelStackWhile = create_label_stack();
  labelStackFor = create_label_stack();
  
  // 抽象構文木を下りながらコード生成
  for (int i = 0; code[i]; i++) {
    gen_function_def(code[i]);
  }
  return 0;
}