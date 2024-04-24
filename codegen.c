#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    error("代入の左辺値が変数ではありません");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {

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