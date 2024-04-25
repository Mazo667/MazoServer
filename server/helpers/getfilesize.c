#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

size_t getFileSize(const char* file_path) {
    FILE* file = fopen(file_path, "rb");
    if (file == NULL) {
        printf("open %s failed.", file_path);
        exit(0);
    }

    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    fclose(file);
    return size;
}