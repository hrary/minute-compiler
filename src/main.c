#include "cc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

char* read_file (const char *filename, long *size) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }
    if (fseek(file, 0, SEEK_END) != 0) {
        perror("Error seeking to end of file");
        fclose(file);
        return NULL;
    }
    long file_size = ftell(file);
    if (file_size == -1) {
        perror("Error getting file size");
        fclose(file);
        return NULL;
    }
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("Error seeking to start of file");
        fclose(file);
        return NULL;
    }
    char *buffer = malloc(file_size + 1);
    if (buffer == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return NULL;
    }
    size_t bytes_read = fread(buffer, 1, file_size, file);
    if (bytes_read < (size_t)file_size) {
        if (ferror(file)) {
            perror("Error reading file");
            free(buffer);
            fclose(file);
            return NULL;
        }
    }

    buffer[bytes_read] = '\0'; 
    fclose(file);

    if (size) {
        *size = (long)bytes_read;
    }

    return buffer;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: mycc <file.c>\n"); return 1; }
    long size = 0;
    char *content = read_file(argv[1], &size);
    if (!content) return 1;
    Token *tok = tokenize(content);
    if (argc > 2 && !strcmp(argv[2], "--dump-tokens")) { dump_tokens(tok); return 0; }
    if (argc > 2 && !strcmp(argv[2], "--dump-ast")) { dump_ast(parse(tok), 0); return 0; }
    Function *fn = parse(tok);
    codegen(fn);
    return 0;
}