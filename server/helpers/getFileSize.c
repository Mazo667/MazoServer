#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

size_t getFileSize(const char* file_path) {
    FILE* fin = fopen(file_path, "rb");
    if (fin == NULL) {
        printf("open %s failed.", file_path);
        exit(0);
    }

    fseek(fin, 0L, SEEK_END);
    size_t size = ftell(fin);
    fclose(fin);
    return size;
}