#include "./georgecc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gen_lvar(Node *node) {
  if (node->kind != ND_LVAR) {
    error("代入の左辺値が変数ではありません");
  }
  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen_gvar(Node *node) {
  if (node->kind != ND_GVAR) {
    error("代入の左辺値が変数ではありません");
  }
  printf("  lea rax, [rip + %.*s]\n", node->len, node->name);
  printf("  push rax\n");
}

void gen_var(Node *node) {
  if (node->kind == ND_DEREF) {
    gen(node->lhs);
    return;
  }
  if (node->kind == ND_LVAR) {
    gen_lvar(node);
  } else if (node->kind == ND_GVAR) {
    gen_gvar(node);
  }
}


void gen_top_level_def(Node *node) {

  if (node->kind == ND_GVAR_DEF) {
    return;
  }

  // 関数プロローグ&変数のメモリ確保
  printf("%.*s:\n", node->len, node->name);
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  int local_var_stack = 0;
  for (LVar *var = node->locals; var; var = var->next) {
    // 最後のoffsetだけでOK (+=ではなくて良い)
    local_var_stack = var->offset;
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
          // TODO ポインタ引数に対応する
          // printf("  mov [rax], rdi\n");
          printf("  mov [rax], edi\n");
          break;
        case 1:
          // TODO ポインタ引数に対応する
          // printf("  mov [rax], rsi\n");
          printf("  mov [rax], esi\n");
          break;
        case 2:
          // TODO ポインタ引数に対応する
          // printf("  mov [rax], rdx\n");
          printf("  mov [rax], edx\n");
          break;
        case 3:
          // TODO ポインタ引数に対応する
          // printf("  mov [rax], rcx\n");
          printf("  mov [rax], ecx\n");
          break;
        case 4:
          // TODO ポインタ引数に対応する
          // printf("  mov [rax], r8\n");
          printf("  mov [rax], r8d\n");
          break;
        case 5:
          // TODO ポインタ引数に対応する
          // printf("  mov [rax], r9\n");
          printf("  mov [rax], r9d\n");
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
      //print_ast(node->branch[i], 0);
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
      gen_var(node);
      printf("  pop rax\n");
      if (node->type->ty == CHAR) {
        printf("  movsx eax, BYTE PTR [rax]\n");
      } else if (node->type->ty == INT) {
        printf("  mov eax, [rax]\n");
      } else if (node->type->ty == ARRAY) {
        printf("  push rax\n");
        return;
      } else {
        if (node->type->ptr_to->ty == CHAR) {
          printf("  mov rax, QWORD PTR [rax]\n");
        } else {
          printf("  mov rax, [rax]\n");  
        }
      }
      printf("  push rax\n");
      return;
    case ND_GVAR:
      gen_var(node);
      printf("  pop rax\n");
      if (node->type->ty == CHAR) {
        printf("  movsx eax, BYTE PTR [rax]\n");
      } else if (node->type->ty == INT) {
        printf("  mov eax, [rax]\n");
      } else if (node->type->ty == ARRAY) {
        printf("  push rax\n");
        return;
      } else {
        if (node->type->ptr_to->ty == CHAR) {
          printf("  mov rax, QWORD PTR [rax]\n");
        } else {
          printf("  mov rax, [rax]\n");  
        }
      }
      printf("  push rax\n");
      return;
    case ND_ASSIGN:
      gen_var(node->lhs);
      if (node->type->ty == PTR && node->type->ptr_to->ty == CHAR) {
        printf("  pop rax\n");
        printf("  push rax\n");
        printf("  mov QWORD PTR [rax], OFFSET FLAT:.LC%d\n", node->rhs->val);
        return;
      }

      gen(node->rhs);

      printf("  pop rdi\n");
      printf("  pop rax\n");

      if (node->type->ty == CHAR) {
        printf("  mov BYTE PTR [rax], rdi\n");
      } else if (node->type->ty == INT) {
        printf("  mov [rax], edi\n");
      } else if (node->type->ty == PTR) {
        printf("  mov [rax], rdi\n");
      } else {
        error("代入に対応していません。");
      }
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
              if (n->type->ty == PTR && n->type->ptr_to->ty == CHAR) {
                printf("  mov rdi, rax\n");
                printf("  mov eax, 0\n");
              } else {
                printf("  pop rdi\n");
              }
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
      printf("  call %.*s\n", node->len, node->name);
      printf("  push rax\n");
      return;
    case ND_ADDR:
      gen_var(node->lhs);
      return;
    case ND_DEREF:
      gen(node->lhs);
      printf("  pop rax\n");
      printf("  mov rax, [rax]\n");
      printf("  push rax\n");
      return;
    case ND_LVAR_DEF:
      return;
    case ND_GVAR_DEF:
      return;
    case ND_STRING:
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


void gen_data() {  
  printf(".data\n");
  for (GVar *var = gvar; var; var = var->next) {
    printf("%.*s:\n", var->len, var->name);
    int size;
    if (var->type->ty == CHAR) {
      size = 1;
    } else if (var->type->ty == INT) {
      size = 4;
    } else if (var->type->ty == PTR) {
      size = 8;
    } else if (var->type->ty == ARRAY) {
      if (var->type->ptr_to->ty == CHAR) {
        size = 1 * var->type->array_size;
      } else if (var->type->ptr_to->ty == INT) {
        size = 4 * var->type->array_size;
      } else if (var->type->ptr_to->ty == PTR) {
        size = 8 * var->type->array_size;
      } else {
        error("get_data 対応していない型です。1");  
      }
    } else {
      error("get_data 対応していない型です。2");
    }
    printf("  .zero %d\n", size);
  }
  for (int i = 0; i < 100; i++) {
    if (chars[i] != NULL) {
      printf(".LC%d:\n", i);
      printf("  .string \"%s\"\n", chars[i]);  
    } else {
      break;
    }
  }
}