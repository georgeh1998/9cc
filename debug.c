#include "./9cc.h"
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
                printf("\n// ");    
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
    printf("// ");
    do {
        printToken(t);
        t = *t.next;
    } while (t.next != NULL);

    printf("\n");
}
