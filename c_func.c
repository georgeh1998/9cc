#include <stdio.h>

int foo() {
    printf("foo called\n");
    return 1;
}

int hoge() {
    int i = 0;
    printf("hoge called\n");
    for (int j = 0; j < 10; j++) {
        i++;
    }
    return i;
}

void call_foo() {
    int a = foo();

    printf("OK call_foo");
}