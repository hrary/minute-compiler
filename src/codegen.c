#include "cc.h"
#include <stdio.h>

static void gen_expr(Node *node) {
    if (node->type == ND_NUM) { printf("  mov $%ld, %%rax\n", node->val); return; }
    gen_expr(node->rhs);
    printf("  push %%rax\n");
    gen_expr(node->lhs);
    printf("  pop %%rdi\n");
    switch (node->type) {
    case ND_ADD: printf("  add %%rdi, %%rax\n"); return;
    case ND_SUB: printf("  sub %%rdi, %%rax\n"); return;
    case ND_MUL: printf("  imul %%rdi, %%rax\n"); return;
    case ND_DIV: printf("  cqo\n  idiv %%rdi\n"); return;
    default: perror("invalid node");
    }
}

void codegen(Node *node) {
    printf("  .text\n");
    printf("  .globl main\n");
    printf("main:\n");
    printf("  push %%rbp\n");
    printf("  mov %%rsp, %%rbp\n");
    gen_expr(node);
    printf("  mov %%rbp, %%rsp\n");
    printf("  pop %%rbp\n");
    printf("  ret\n");
}