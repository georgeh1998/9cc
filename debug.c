#include "./georgecc.h"
#include <stdio.h>
#include <string.h>


char* substring(char *result, char *c, int len) {
    for (int i = 0; i < len; i++) {
        result[i] = c[i];
    }
    result[len] = '\0';
    return result;
}

void printToken(Token token) {
    char *result;
    switch (token.kind) {
        case TK_RESERVED:
            substring(result, token.str, token.len);;
            if (result[0] == ';') {
                printf("%s", result);
                printf("\n ");    
            } else {
                printf("%s", result);
                printf(" -> ");    
            }
            break;
        case TK_NUM:
            printf("%d", token.val);
            printf(" -> ");
            break;
        case TK_IDENT:
        case TK_RETURN:
        case TK_IF:
        case TK_ELSE:
        case TK_WHILE:
            substring(result, token.str, token.len);;
            printf("%s", result);
            printf(" -> ");
            break;
        case TK_EOF:
            printf("EOF");
            break;
    }
}

// Debug用関数

void printTokens(Token t) {
    printf("; ");
    do {
        printToken(t);
        t = *t.next;
    } while (t.next != NULL);

    printf("\n");
}


// ASTを出力する関数
void print_ast(Node *node, int depth) {
    if (node == NULL) return;

    // 右の子ノードを先に出力
    print_ast(node->rhs, depth + 1);

    // 現在のノードを出力
    for (int i = 0; i < depth; i++) {
        printf("    ");
    }

    switch (node->kind) {
    case ND_LVAR:
        printf("%.*s\n", node->len, node->name);
        break;
    case ND_ASSIGN:
        printf("=(%d)\n", node->type->ty);
        break;
    case ND_DEREF:
        printf("*(%d)\n", node->type->ty);
        break;
    case ND_ADD:
        printf("+(%d)\n", node->type->ty);
        break;
    case ND_SUB:
        printf("-\n");
        break;
    case ND_MUL:
        printf("*(%d)\n", node->type->ty);
        break;
    case ND_NUM:
        printf("%d\n", node->val);
        break;
    case ND_STRING:
        printf("%.*s\n", node->len, node->name);
        break;
    case ND_RETURN:
        printf("戻\n");
        break;
    default:
        printf("Unknown NodeKind: %d\n", node->kind);
    }

    // 左の子ノードを出力
    print_ast(node->lhs, depth + 1);
}