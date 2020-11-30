#ifndef FPACK_H
#define FPACK_H

#include <stdlib.h>
#include <stdio.h>

typedef struct file_s
{
    char *name;
    char *mode;
    FILE *fp;
    long size;
} file_s;

file_s* open_file(char* name, char *mode);

long file_size(FILE *fp);

void close_file(file_s *fp);

#endif
