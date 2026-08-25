#include "cc.h"

static bool is_keyword(Token *tok) {
    static char *kw[] = {"return", "int", "if", "else", "while", "for"};
    for (size_t i = 0; i < sizeof(kw)/sizeof(*kw); i++)
        if (tok->len == (int)strlen(kw[i]) && !strncmp(tok->loc, kw[i], tok->len))
            return true;
    return false;
}

static Token *new_token(TokenType type, const char *start, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->type = type;
    tok->loc = start;
    tok->len = len;
    return tok;
}

static void convert_keywords(Token *tok) {
    for (Token *t = tok; t; t = t->next)
        if (t->type == TK_IDENT && is_keyword(t))
            t->type = TK_KEYWORD;
}

Token *tokenize(char *content) {
    Token head = {0};
    Token *cur = &head;
    char *p = content;

    while (*p) {
        if (isspace((unsigned char)*p)) {
            p++;
            continue;
        }

        if (isdigit((unsigned char)(*p))) {
            char *start = p;
            long val = strtol(p, &p, 10);
            cur = cur->next = new_token(TK_NUM, start, p - start);
            cur->val = val;
            continue;
        }

        if (isalpha((unsigned char)*p) || *p == '_') {
            char *start = p;
            while (isalnum((unsigned char)*p) || *p == '_') p++;
            cur = cur->next = new_token(TK_IDENT, start, p - start);
            continue;
        }

        if (strchr("+-*/(){};=", *p)) {
            cur = cur->next = new_token(TK_PUNCT, p, 1);
            p++;
            continue;
        }

        fprintf(stderr, "invalid character: '%c'\n", *p);
    }

    cur->next = new_token(TK_EOF, p, 0);
    convert_keywords(head.next);
    return head.next;
}

void dump_tokens(Token *tok) {
    static const char *names[] = {"NUM", "PUNCT", "IDENT", "KEYWORD", "EOF"};
    for (Token *t = tok; t; t = t->next)
        fprintf(stderr, "%-8s '%.*s'%s\n", names[t->type], t->len, t->loc,
                t->type == TK_NUM ? " " : "");
    fprintf(stderr, "---\n");
}
