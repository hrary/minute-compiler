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

    if (argc < 3) { 
        fprintf(stderr, "usage: mycc <input.c> <output.s>\n"); 
        return 1; 
    }

    long size = 0;
    char *content = read_file(argv[1], &size);
    if (content == NULL) {
        fprintf(stderr, "Error reading file: %s\n", argv[1]);
        return 1; 
    }
    fprintf(stdout, "File touched: %s", argv[1]);

    fprintf(stdout, "File size: %ld bytes\n", size);
    fprintf(stdout, "File content:\n%s\n", content);

    long i;
    int start = -1;
    for (i=0; i<size; i++) {
        if (isdigit((unsigned char)content[i])) {
            start = i;
            while (i < size && isdigit((unsigned char)content[i])) i++;
            break;
        }
    }
    if (start == -1) {
        fprintf(stderr, "no numbers found\n");
        return -1;
    }
    long number = strtol(content + start, NULL, 10);

    FILE *output_file = fopen(argv[2], "w");
    if (output_file == NULL) {
        perror("Error opening file\n");
        return 1; 
    }
    fprintf(output_file, 
        "\t.text\n"
        "\t.globl main\n"
        "main:\n"
        "\tpush %%rbp\n"
        "\tmov %%rsp, %%rbp\n"
        "\tmov $%ld, %%rax\n"
        "\tpop %%rbp\n"
        "\tret\n", number);

    fclose(output_file);
    fprintf(stdout, "File written: %s\n", argv[2]);
    free(content);

    return 0;
}