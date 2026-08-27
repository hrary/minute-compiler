#include "cc.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>

static char *resolve_filename(const char *input) {
    FILE *f = fopen(input, "r");
    if (f) {
        fclose(f);
        return strdup(input);
    }

    const char *last_slash = strrchr(input, '/');
    char dir_path[256] = ".";
    const char *prefix = input;

    if (last_slash) {
        size_t dir_len = last_slash - input;
        if (dir_len >= sizeof(dir_path)) return strdup(input);
        
        strncpy(dir_path, input, dir_len);
        dir_path[dir_len] = '\0';
        prefix = last_slash + 1;
    }

    DIR *dir = opendir(dir_path[0] ? dir_path : ".");
    if (!dir) return strdup(input);

    struct dirent *entry;
    char *matched_name = NULL;
    size_t prefix_len = strlen(prefix);

    while ((entry = readdir(dir)) != NULL) {
        size_t len = strlen(entry->d_name);
        if (len >= prefix_len + 2 &&
            strncmp(entry->d_name, prefix, prefix_len) == 0 &&
            strcmp(entry->d_name + len - 2, ".c") == 0) {
            
            size_t path_len = strlen(dir_path) + 1 + len + 1;
            matched_name = malloc(path_len);
            if (matched_name) {
                if (last_slash) {
                    snprintf(matched_name, path_len, "%s/%s", dir_path, entry->d_name);
                } else {
                    snprintf(matched_name, path_len, "%s", entry->d_name);
                }
            }
            break;
        }
    }

    closedir(dir);
    return matched_name ? matched_name : strdup(input);
}


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
    if (argc < 2) { fprintf(stderr, "usage: mycc <file.c|prefix>\n"); return 1; }

    char *target_file = resolve_filename(argv[1]);
    long size = 0;
    char *content = read_file(target_file, &size);
    if (!content) return 1;
     
    Token *tok = tokenize(content);
    if (argc > 2 && !strcmp(argv[2], "--dump-tokens")) { dump_tokens(tok); return 0; }
    if (argc > 2 && !strcmp(argv[2], "--dump-ast")) { dump_ast(parse(tok), 0); return 0; }
    Function *fn = parse(tok);
    codegen(fn);
    return 0;
}