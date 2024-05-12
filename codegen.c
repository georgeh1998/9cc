#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gen_lval(Node *node) {
  if (node->kind == ND_DEREF) {
    gen(node->lhs);
    return;
  }

  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");
  
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen_function_def(Node *node) {
  // 関数ラベルを出力
  char label[node->len];
  for (int i = 0; i < node->len; i++) {
    label[i] = node->name[i];
  }
  label[node->len] = '\0';

  // 関数プロローグ&変数のメモリ確保
  printf("%s:\n", label);
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  int local_var_stack = 0;
  for (LVar *var = node->locals; var; var = var->next) {
    local_var_stack += 8;
  }
  printf("  sub rsp, %d\n", local_var_stack);

  // 関数の引数のメモリ割り当て
  int i_arg = 0;
  for (LVar *var = node->locals; var; var = var->next) {
    if (var->is_arg) {
      printf("  mov rax, rbp\n");
      printf("  sub rax, %d\n", var->offset);
      printf("  push rax\n");
      switch (i_arg) {
        case 0:
          printf("  mov [rax], rdi\n");
          break;
        case 1:
          printf("  mov [rax], rsi\n");
          break;
        case 2:
          printf("  mov [rax], rdx\n");
          break;
        case 3:
          printf("  mov [rax], rcx\n");
          break;
        case 4:
          printf("  mov [rax], r8\n");
          break;
        case 5:
          printf("  mov [rax], r9\n");
          break;
      }
      i_arg++;
    } else {
      break;
    }
  }

  // 関数のブロック
  for (int i = 0; true; i++) {
    if (node->branch[i]) {
      gen(node->branch[i]);
    } else {
      break;
    }
  }

  printf("  pop rax\n");

  // 関数エピローグ
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

void gen(Node *node) {

  char result[node->len];

  switch (node->kind) {
    case ND_NUM:
      printf("  push %d\n", node->val);
      return;
    case ND_LVAR:
      gen_lval(node);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_lval(node->lhs);
      gen(node->rhs);

      printf("  pop rdi\n");
      printf("  pop rax\n");
      printf("  mov [rax], rdi\n");
      printf("  push rdi\n");
      return;
    case ND_RETURN:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rsp, rbp\n");
      printf("  pop rbp\n");
      printf("  ret\n");
      return;
    case ND_IF:
      int if_label = push_label(labelStackIf);
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .ELSE%d\n", if_label);
      if (node->branch[1]) {
        gen(node->branch[0]);
        printf("  jmp .ENDIF%d\n", if_label);
        printf(".ELSE%d:\n", if_label);
        gen(node->branch[1]);
        printf(".ENDIF%d:\n", if_label);
      } else {
        gen(node->branch[0]);
        printf(".ELSE%d:\n", if_label);
      }
      int pop = pop_label(labelStackIf);
      return;
    case ND_WHILE:
      int while_label = push_label(labelStackWhile);
      printf(".WHILESTART%d:\n", while_label);
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .WHILEEND%d\n", while_label);
      gen(node->rhs);
      printf("  jmp .WHILESTART%d\n", while_label);
      printf(".WHILEEND%d:\n", while_label);
      pop_label(labelStackWhile);
      return;
    case ND_FOR:
      int for_label = push_label(labelStackFor);
      gen(node->branch[0]);
      printf(".FORSTART%d:\n", for_label);
      gen(node->branch[1]);
      printf("  pop rax\n");
      printf("  cmp rax, 0\n");
      printf("  je .FOREND%d\n", for_label);
      gen(node->branch[3]);
      gen(node->branch[2]);
      printf("  jmp .FORSTART%d\n", for_label);
      printf(".FOREND%d:\n", for_label);
      pop_label(labelStackFor);
      return;
    case ND_BLOCK:
      int i = 0;
      for (;;) {
        Node *n = node->branch[i];
        if (n) {
          gen(n);
          i += 1;
        } else {
          break;
        }
      }
      return;
    case ND_FUNC_CALL:
      int arg_i = 0;
      for (;;) {
        Node *n = node->branch[arg_i];
        if (n) {
          gen(n);
          switch (arg_i) {
            case 0:
              printf("  pop rdi\n");
              break;
            case 1:
              printf("  pop rsi\n");
              break;
            case 2:
              printf("  pop rdx\n");
              break;
            case 3:
              printf("  pop rcx\n");
              break;
            case 4:
              printf("  pop r8\n");
              break;
            case 5:
              printf("  pop r9\n");
              break;
          }
          arg_i++;
        } else {
          break;
        }
      }
      strncpy(result, node->name, node->len);
      result[node->len] = '\0';
      printf("  call %s\n", result);
      printf("  push rax\n");
      return;
    case ND_ADDR:
      gen_lval(node->lhs);
      return;
    case ND_DEREF:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_LVAR_DEF:
      return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;
    case ND_EQUAL:
      printf("  cmp rax, rdi\n");
      printf("  sete al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_NEQUAL:
      printf("  cmp rax, rdi\n");
      printf("  setne al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_GT:
      printf("  cmp rax, rdi\n");
      printf("  setg al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_EGT:
      printf("  cmp rax, rdi\n");
      printf("  setge al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_LT:
      printf("  cmp rax, rdi\n");
      printf("  setl al\n");
      printf("  movzb rax, al\n");
      break;
    case ND_ELT:
      printf("  cmp rax, rdi\n");
      printf("  setle al\n");
      printf("  movzb rax, al\n");
      break;
  }

  printf("  push rax\n");
}