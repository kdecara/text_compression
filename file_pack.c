#include <stdlib.h>
#include <stdio.h>
#include "file_pack.h"

file_s* open_file(char* name, char *mode)
{	
    FILE *ptr;
    file_s *file = (file_s*)malloc(sizeof(file_s));
    printf("opening file %s...\n", name);
    ptr = fopen(name, mode);
    if(ptr == NULL)
    {
	printf("fp not found");
	exit(1);
    }
    file->size = file_size(ptr);
    file->fp = ptr;
    file->name = name;
    file->mode = mode;
    printf("file %s opened\n", name);
    return file;
}

long file_size(FILE *fp)
{
    long size;
    printf("getting file size...\n");
    if(fp == NULL) return -1;
    if(fseek(fp, 0L, SEEK_END) == 0)
    {
	size = ftell(fp);
	if(size == -1) exit(1);
    }
    if(fseek(fp, 0L, SEEK_SET) != 0) exit(1);
    printf("file is %ld bytes\n", size);
    return size;
}

void close_file(file_s *file)
{
    printf("closing file %s...\n", file->name);
    fclose(file->fp);
    /*free(file->name);
    free(file->mode);*/
    free(file);
    printf("file closed\n");
}

