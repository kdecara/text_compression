#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dynamic_array.h"
/*data type for array is presumed to be unsigned char, and size will be long, since we are dealing
 *with files*/
Buffer* init_buffer(long file_len, FILE *fp)
{
    Buffer *buffer = (Buffer*)malloc(sizeof(Buffer));
    buffer->data = (unsigned char*)malloc(sizeof(unsigned char) * file_len);
    fread(buffer->data, file_len, 1, fp);
    buffer->used = file_len;
    buffer->size = file_len;
    if(buffer == NULL) exit(1);
    return buffer;
}

int grow(Buffer *buffer)
{
    buffer->size *= 2;
    buffer->data = (unsigned char*)realloc(buffer->data, buffer->size * sizeof(unsigned char));
    if(buffer->data == NULL) exit(1);
    return 1;    
}

int delete(Buffer *buffer, int i)
{
    /*allocate an array with size - 1*/
    unsigned char* temp = (unsigned char*)malloc((buffer->used - 1) * sizeof(unsigned char));
    if(i >= buffer->used || i < 0) return 0;
    /*use memmove to copy everything before the index*/ 
    memmove(temp, buffer->data, (i+1)*sizeof(unsigned char));
    /*use memmove to copy everything after the index*/
    memmove(temp+i, (buffer->data)+(i+1), (buffer->used - i) * sizeof(unsigned char));
    free(buffer->data);
    buffer->data = temp;
    buffer->used = buffer->used - 1;
    buffer->size = buffer->size - 1;
    /*can trim here or in main file to save on runtime (calling trim() here is more costly) trim(buffer);*/ 
    return 1;
}

int insert(Buffer *buffer, unsigned char new_val, int i)
{
    unsigned char *temp = (unsigned char*)malloc((buffer->used + 1) * sizeof(unsigned char));
    if(is_full(buffer)) grow(buffer);
    if(i < 0) return 0;
    else if(i >= buffer->used)
    {
	memmove(temp, buffer->data, (buffer->used) * sizeof(unsigned char));
	temp[buffer->used] = new_val;
	free(buffer->data);
	buffer->data = temp;
	buffer->used += 1;
	buffer->size += 1;
	return 1;
    }
    memmove(temp, buffer->data, (i+1) * sizeof(unsigned char));
    temp[i] = new_val;
    memmove(temp+i+1,(buffer->data)+(i), (buffer->used - i + 1) * sizeof(unsigned char));
    free(buffer->data);
    buffer->data = temp;
    buffer->used += 1;
    buffer->size += 1;
    return 1;
}

int trim(Buffer* buffer)
{
    int new_size = buffer->used;
    if(buffer->size - new_size <= 0) return -1;
    buffer->data = (unsigned char*)realloc(buffer->data, sizeof(unsigned char) * new_size);
    if(buffer->data == NULL) exit(1);
    buffer->size = buffer->used;
    return 1; 
}

void delete_buffer(Buffer *buffer)
{
    free(buffer->data);
    buffer->data = NULL;
    buffer->used = 0;
    buffer->size = 0;
}

void print_buffer(Buffer buffer)
{
    int i, max = buffer.used;
    printf("used: %ld\n", buffer.used);
    printf("size: %ld\n", buffer.size);
    for(i = 0; i < max; i++)
    {
	printf("buffer[%d] = %d\n", i, max); 
    }
}

int is_full(Buffer *buffer)
{
    return buffer->used == buffer->size;
}
