#include "cc.h"

static Node *add(Token **rest, Token *tok);
static Node *mul(Token **rest, Token *tok);
static Node *primary(Token **rest, Token *tok);
static Node *assign(Token **rest, Token *tok);
static Node *expr(Token **rest, Token *tok);
static void assign_offsets(Function *fn);

static Function *current_fn;

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

static Obj *find_var(Token *tok) {
    for (Obj *var = current_fn->locals; var; var = var->next)
        if ((int)strlen(var->name) == tok->len && !strncmp(var->name, tok->loc, tok->len))
            return var;
    return NULL;
}

static Obj *new_lvar(const char *name, int len) {
    Obj *var = calloc(1, sizeof(Obj));
    var->name = strndup(name, len);
    var->next = current_fn->locals;
    current_fn->locals = var;
    return var;
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

static Node *assign(Token **rest, Token *tok) {
    Node *node = add(&tok, tok);
    if (equal(tok, "=")) {
        node = new_binary(ND_ASSIGN, node, assign(&tok, tok->next));
    }
    *rest = tok;
    return node;
}

static Node *declaration(Token **rest, Token *tok) {
    tok = skip(tok, "int");
    if (tok->type != TK_IDENT) error("expected a variable name");
    if (find_var(tok)) error("redefinition of '%.*s'", tok->len, tok->loc);
    Obj *var = new_lvar(tok->loc, tok->len);
    Token *name_tok = tok;
    tok = tok->next;

    if (!equal(tok, ";")) {
        tok = skip(tok, "=");
        Node *lhs = new_node(ND_VAR);
        lhs->var = var;
        Node *rhs = assign(&tok, tok);
        Node *node = new_node(ND_EXPR_STMT);
        node->lhs = new_binary(ND_ASSIGN, lhs, rhs);
        *rest = skip(tok, ";");
        return node;
    }
    (void)name_tok;
    *rest = skip(tok, ";");
    return new_node(ND_EXPR_STMT);
}

static Node *add(Token **rest, Token *tok) {
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

static Node *expr(Token **rest, Token *tok) {
    Node *node = assign(&tok, tok);
    *rest = tok;
    return node;
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
    if (tok->type == TK_IDENT) {
        Obj *var = find_var(tok);
        if (!var) error("undefined variable '%.*s'", tok->len, tok->loc);
        Node *node = new_node(ND_VAR);
        node->var = var;
        *rest = tok->next;
        return node;
    }
    error("expected an expression");
    return NULL;
}

static Node *stmt(Token **rest, Token *tok) {
    if (equal(tok, "int")) return declaration(rest, tok);
    if (equal(tok, "return")) {
        Node *node = new_node(ND_RETURN);
        node->lhs = expr(&tok, tok->next);
        *rest = skip(tok, ";");
        return node;
    }
    Node *node = new_node(ND_EXPR_STMT);
    node->lhs = expr(&tok, tok);
    *rest = skip(tok, ";");
    return node;
}

Function *parse(Token *tok) {
    Function *fn = calloc(1, sizeof(Function));
    current_fn = fn;

    tok = skip(tok, "int");
    tok = tok->next;
    tok = skip(tok, "(");
    tok = skip(tok, ")");
    tok = skip(tok, "{");

    Node head = {0};
    Node *cur = &head;
    while (!equal(tok, "}"))
        cur = cur->next = stmt(&tok, tok);
    fn->body = head.next;
    assign_offsets(fn);
    return fn;
}

static void assign_offsets(Function *fn) {
    int offset = 0;
    for (Obj *var = fn->locals; var; var = var->next) {
        offset += 8;
        var->offset = -offset;
    }
    fn->size = (offset + 15) / 16 * 16;
}

void dump_ast_node(Node *node, int depth) {
    if (!node) return;
    static const char *names[] = {"ADD", "SUB", "MUL", "DIV", "NUM", "VAR", "ASSIGN", "RETURN", "EXPR_STMT"};
    fprintf(stderr, "%*s%s", depth * 2, "", names[node->type]);
    if (node->type == ND_NUM) fprintf(stderr, " %ld", node->val);
    fprintf(stderr, "\n");
    dump_ast_node(node->lhs, depth + 1);
    dump_ast_node(node->rhs, depth + 1);
    dump_ast_node(node->next, depth);
}

void dump_ast(Function *fn, int depth) {
    if (!fn) return;
    dump_ast_node(fn->body, depth);
}