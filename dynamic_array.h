#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdlib.h>
#include <stdio.h>


typedef struct dynamic_array_s
{
    unsigned char* data;
    long used;
    long size;
} Buffer;

Buffer* init_buffer(long file_len, FILE *fp);

int grow(Buffer *buffer);

int delete(Buffer *buffer, int i);

int trim(Buffer *buffer);

void delete_buffer(Buffer *buffer);

int insert(Buffer *buffer, unsigned char data, int i);

void print_buffer(Buffer buffer);

int is_full(Buffer *buffer);

#endif

