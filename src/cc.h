#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
    TK_NUM,
    TK_PUNCT,
    TK_IDENT,
    TK_KEYWORD,
    TK_EOF
} TokenType;

typedef struct Token {
    TokenType type;
    struct Token *next;
    long val;
    const char *loc;
    int len;
} Token;

typedef enum {
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,
    ND_NUM,
    ND_VAR,
    ND_ASSIGN,
    ND_RETURN,
    ND_EXPR_STMT
} NodeType;

typedef struct Obj {
    struct Obj *next;
    char *name;
    int offset;
} Obj;

typedef struct Node {
    NodeType type;
    struct Node *lhs, *rhs, *next;
    long val;
    Obj *var;
} Node;

typedef struct Function {
    Node *body;
    Obj *locals;
    int size;
} Function;

Token *tokenize(char *content);
void dump_tokens(Token *tok);
void dump_ast(Function *fn, int depth);
Function *parse(Token *tok);
void codegen(Function *fn);
char *read_file(const char *path, long *size);
void error(const char *fmt, ...);
bool equal(Token *tok, const char *op);
Token *skip(Token *tok, const char *op);
