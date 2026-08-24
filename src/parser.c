#include "cc.h"

static Node *expr(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);

void error(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool equal(Token *tok, const char *op) {
    return tok->len == (int)strlen(op) && !strncmp(tok->loc, op, tok->len);
}

Token *skip(Token *tok, const char *op) {
    if (!equal(tok, op)) error("expected '%s'", op);
    return tok->next;
}

static Node *new_node(NodeType type) {
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    return node;
}

static Node *new_binary(NodeType type, Node *lhs, Node *rhs) {
    Node *node = new_node(type);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static Node *new_node_num(long val) {
    Node *node = new_node(ND_NUM);
    node->val = val;
    return node;
}

static Node *expr(Token **rest, Token *tok) {
    Node *node = mul(&tok, tok);
    for (;;) {
        if (equal(tok, "+")) {
            node = new_binary(ND_ADD, node, mul(&tok, tok->next));
            continue;
        }
        else if (equal(tok, "-")) {
            node = new_binary(ND_SUB, node, mul(&tok, tok->next));
            continue;
        }
        *rest = tok;
        return node;
    }
}

static Node *mul(Token **rest, Token *tok) {
    Node *node = primary(&tok, tok);
    for (;;) {
        if (equal(tok, "*")) {
            node = new_binary(ND_MUL, node, primary(&tok, tok->next));
            continue;
        }
        else if (equal(tok, "/")) {
            node = new_binary(ND_DIV, node, primary(&tok, tok->next));
            continue;
        }
        *rest = tok;
        return node;
    }
}

static Node *primary(Token **rest, Token *tok) {
    if (equal(tok, "(")) {
        Node *node = expr(&tok, tok->next);
        *rest = skip(tok, ")");
        return node;
    }
    if (tok->type == TK_NUM) {
        Node *node = new_node_num(tok->val);
        *rest = tok->next;
        return node;
    }
    error("expected an expression");
    return NULL;
}

Node *parse(Token *tok) {
    while (tok->type != TK_EOF && !equal(tok, "return"))
        tok = tok->next;
    if (tok->type == TK_EOF) error("no return statement found");
    Node *node = expr(&tok, tok->next);
    return node;
}

void dump_ast(Node *node, int depth) {
    if (!node) return;
    static const char *names[] = {"ADD", "SUB", "MUL", "DIV", "NUM"};
    fprintf(stderr, "%*s%s", depth * 2, "", names[node->type]);
    if (node->type == ND_NUM) fprintf(stderr, " %ld", node->val);
    fprintf(stderr, "\n");
    dump_ast(node->lhs, depth + 1);
    dump_ast(node->rhs, depth + 1);
}