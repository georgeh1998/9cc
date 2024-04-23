#include "./9cc.h"
#include <stdlib.h>
#include <stdio.h>

LabelStack *create_label_stack() {
    LabelStack *stack = calloc(1, sizeof(LabelStack));
    stack->top = -1;
    stack->currentLabel = -1;
}

int push_label(LabelStack *stack) {
    stack->top += 1;
    stack->currentLabel += 1;
    stack->stack[stack->top] = stack->currentLabel;
    return stack->currentLabel;
}

int pop_label(LabelStack *stack) {
    int popValue = stack->stack[stack->top];
    stack->top -= 1;
    return popValue;
}