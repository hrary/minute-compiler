#include "cc.h"
#include <stdio.h>

static void gen_expr(Node *node);
static void gen_stmt(Node *node);
static char *argreg[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
static Function *current_fn;

static void gen_addr(Node *node) {
    if (node->type == ND_VAR) {
        printf("  lea %d(%%rbp), %%rax\n", node->var->offset);
        return;
    }
    error("not an lvalue");
}

static int counter(void) {
    static int i = 0;
    return ++i;
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
    case ND_FUNCALL: {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_expr(arg);
            printf("  push %%rax\n");
            nargs++;
        }
        for (int i = nargs - 1; i >= 0; i--)
            printf("  pop %s\n", argreg[i]);
        printf("  mov $0, %%rax\n");
        printf("  call %s\n", node->funcname);
        return;
    }
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
    case ND_EQ: printf("  cmp %%rdi, %%rax\n  sete %%al\n  movzb %%al, %%rax\n"); return;
    case ND_NE: printf("  cmp %%rdi, %%rax\n  setne %%al\n  movzb %%al, %%rax\n"); return;
    case ND_LT: printf("  cmp %%rdi, %%rax\n  setl %%al\n  movzb %%al, %%rax\n"); return;
    case ND_LE: printf("  cmp %%rdi, %%rax\n  setle %%al\n  movzb %%al, %%rax\n"); return;
    default: perror("invalid node");
    }
}


static void gen_stmt(Node *node) {
    switch (node->type) {
    case ND_RETURN:
        gen_expr(node->lhs);
        printf("  jmp .L.return.%s\n", current_fn->name);
        return;
    case ND_EXPR_STMT:
        if (node->lhs) gen_expr(node->lhs);
        return;
    case ND_IF: {
        int c = counter();
        gen_expr(node->cond);
        printf("  cmp $0, %%rax\n");
        printf("  je .L.else.%d\n", c);
        gen_stmt(node->then);
        printf("  jmp .L.end.%d\n", c);
        printf(".L.else.%d:\n", c);
        if (node->els) gen_stmt(node->els);
        printf(".L.end.%d:\n", c);
        return;
    }
    case ND_FOR: {
        int c = counter();
        if (node->init) gen_stmt(node->init);
        printf(".L.begin.%d:\n", c);
        if (node->cond) {
            gen_expr(node->cond);
            printf("  cmp $0, %%rax\n");
            printf("  je .L.end.%d\n", c);
        }
        gen_stmt(node->then);
        if (node->inc) gen_expr(node->inc);
        printf("  jmp .L.begin.%d\n", c);
        printf(".L.end.%d:\n", c);
        return;
    }
    case ND_BLOCK: {
        for (Node *n = node->body; n; n = n->next)
            gen_stmt(n);
        return;
    }
    default:
        error("invalid statement");
    }
}

void codegen(Function *fn) {
    printf("  .text\n");
    for (Function *f = fn; f; f = f->next) {
        current_fn = f;
        printf("  .globl %s\n", f->name);
        printf("%s:\n", f->name);
        printf("  push %%rbp\n");
        printf("  mov %%rsp, %%rbp\n");
        printf("  sub $%d, %%rsp\n", f->size);
        
        int i = 0;
        for (Obj *p = f->params; p; p = p->param_next)
            printf("  mov %s, %d(%%rbp)\n", argreg[i++], p->offset);

        for (Node *n = f->body; n; n = n->next)
            gen_stmt(n);

        printf(".L.return.%s:\n", f->name);
        printf("  mov %%rbp, %%rsp\n");
        printf("  pop %%rbp\n");
        printf("  ret\n");
    }
}