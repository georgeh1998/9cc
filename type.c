#include "./9cc.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Type *get_base_type() {
    Type *type = calloc(1, sizeof(Type));
    if (is_type("char")) {
        type->ty = CHAR;
    } else if (is_type("int")) {
        type->ty = INT;
    } else {
        error("対応していない型の宣言です。");
    }
    return type;
}
