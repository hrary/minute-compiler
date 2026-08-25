#include "cc.h"
#include <stdio.h>

static void gen_addr(Node *node) {
    if (node->type == ND_VAR) {
        printf("  lea %d(%%rbp), %%rax\n", node->var->offset);
        return;
    }
    error("not an lvalue");
}

static void gen_expr(Node *node) {
    switch (node->type) {
    case ND_NUM:    printf("  mov $%ld, %%rax\n", node->val); return;
    case ND_VAR:    gen_addr(node); printf("  mov (%%rax), %%rax\n"); return;
    case ND_ASSIGN:
        gen_addr(node->lhs);
        printf("  push %%rax\n");
        gen_expr(node->rhs);
        printf("  pop %%rdi\n");
        printf("  mov %%rax, (%%rdi)\n");
        return;
    default: break;
    }
    
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


static void gen_stmt(Node *node) {
    switch (node->type) {
    case ND_RETURN:
        gen_expr(node->lhs);
        printf("  jmp .L.return\n");
        return;
    case ND_EXPR_STMT:
        if (node->lhs) gen_expr(node->lhs);
        return;
    default:
        error("invalid statement");
    }
}

void codegen(Function *fn) {
    printf("  .text\n");
    printf("  .globl main\n");
    printf("main:\n");
    printf("  push %%rbp\n");
    printf("  mov %%rsp, %%rbp\n");
    printf("  sub $%d, %%rsp\n", fn->size);

    for (Node *n = fn->body; n; n = n->next)
        gen_stmt(n);

    printf(".L.return:\n");
    printf("  mov %%rbp, %%rsp\n");
    printf("  pop %%rbp\n");
    printf("  ret\n");
}