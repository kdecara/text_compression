#include <stdio.h>
#include <stdlib.h>
#include "file_pack.h"
#include "dynamic_array.h"
#define BYTE_TO_BINARY_PATTERN "%d%d%d%d%d%d%d%d"

#define BYTE_TO_BINARY(x) \
	(x & 0x80 ? 1 : 0), \
	(x & 0x40 ? 1 : 0), \
	(x & 0x20 ? 1 : 0), \
	(x & 0x10 ? 1 : 0), \
	(x & 0x08 ? 1 : 0), \
	(x & 0x04 ? 1 : 0), \
	(x & 0x02 ? 1 : 0), \
	(x & 0x01 ? 1 : 0)

#define PRINT_BIN(x) printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(x));

#define EXIT_PROGRAM(string){\
	printf("%s\n", string);\
	printf("program will now exit\n"); \
	exit(1); \
}

#define CHECK_NULL(object) ((object) == NULL ? 1 : 0)

#define FREE(object) {\
	if(object != NULL) free(object);\
	else printf("attempted to free NULL object\n"); \
}

#define CHECK_ALLOC(object) {\
	if(CHECK_NULL(object)){\
		EXIT_PROGRAM("Memory allocation failed");\
	}\
}

typedef unsigned char byte;
int pack(byte *buffer, FILE* unpacked_in, long buffer_size);
int pack_bits(Buffer *buffer);
byte get_least_bits(byte b, int index);
byte get_sig_bits(byte b, int index);
byte format_byte(byte b, int shift_val);
int unpack_bits(Buffer *buffer);
void test(Buffer *buffer);

int main(int argc, char *argv[])
{
	
    file_s *unpacked_in = open_file(argv[1], "r");
    file_s *compressed_out = open_file(argv[2], "wb");
    Buffer *buffer = init_buffer(unpacked_in->size, unpacked_in->fp);
    long num_unpacked_bytes, num_packed_bytes, diff;
    double percent_saved;
    if(argc != 3) EXIT_PROGRAM("format: [program name] [ascii_in_file] [compressed_out_file]");

    printf("buffer: \n");
    test(buffer);
    pack_bits(buffer);
    trim(buffer);
    printf("packed buffer: \n");
    test(buffer);
 
    fwrite(buffer->data, buffer->used, 1, compressed_out->fp);

    unpack_bits(buffer);
    printf("unpacked buffer: \n");
    test(buffer);

    num_unpacked_bytes = unpacked_in->size;
    num_packed_bytes = compressed_out->size;
    diff = num_unpacked_bytes - num_packed_bytes;
    percent_saved = (diff/num_unpacked_bytes) * 100;
    printf("file size reduced by %ld bytes or %f %% \n", diff, percent_saved);

    delete_buffer(buffer);
    close_file(unpacked_in);
    close_file(compressed_out);

    return 0;
}

/*TODO: this function may be easier to write recursively. Try another time*/
int unpack_bits(Buffer *buffer)
{
    int i = 0, current = 0, shift_val = 0, turn = 0;
    byte packed_bits[] = {0, 0};
    printf("unpacking...\n");
    while(1)
    {
	for(i = 0; i < 8; i++)
	{
	    if(buffer->used == current) return 1;
	    shift_val = 8 - (i + 1);
	    packed_bits[turn] = get_sig_bits(buffer->data[current], i);
	    packed_bits[turn] = packed_bits[turn] >> shift_val;
	    buffer->data[current] = buffer->data[current] << i;
	    buffer->data[current] = buffer->data[current] | packed_bits[!turn];
	    buffer->data[current] = buffer->data[current] & 127;
	    packed_bits[!turn] = packed_bits[turn]; 
	    current++;
	}
	printf("inserting %c = %d at %d\n", packed_bits[!turn], packed_bits[!turn], current); 
	insert(buffer, packed_bits[!turn], current);
	packed_bits[0] = 0; packed_bits[1] = 0;
	turn = 0;

    }
    return 1;
}
int pack_bits(Buffer *buffer)
{  
    int i = 0, current = 0, shift_val = 0;
    byte lsbs_next_byte;
    printf("packing...\n");
    while(1)
    {
        for(i = 0; i < 8; i++)
        {
	    /*if there is no next byte to pack into the current one, shift the current bits and stop*/
	    buffer->data[current] = buffer->data[current] >> i;
	    if(current == buffer->used)
	    {
		trim(buffer);
	        return 1;
	    }
	    /*otherwise, continue the packing cycle, 1 extract bits to pack from next byte
	     *2 pack them into the current byte*/
	    /*1 extract the bits from the next byte*/
	    lsbs_next_byte = get_least_bits(buffer->data[current+1], i);
	    shift_val = 8 - (i+1);
	    lsbs_next_byte = lsbs_next_byte << shift_val;
	    /*2 pack them into the current byte*/
	    buffer->data[current] = buffer->data[current] | lsbs_next_byte;
	    current++;
        }
	/*on the i = 8 iteration, the entire byte has been preserved on i = 7, so we can delete this*/
	current--;
	delete(buffer, current);
    }    
    return 0;
}
/*
 * given a byte of the form 11110000 if we want the 3 sig bits 
 * this function returns 11100000
 */
byte get_sig_bits(byte b, int index)
{
    int shift_val = 8 - (index + 1);
    byte a = 255 << shift_val, b1 = b & a;
    if(index < 0) return b;
    return b1;
}

/*
 * b = byte; index = (index of most-sig-bit of b that we want), b1 = returned-bits
 * e.g. if we want the 2 least bits of 00001111 this function returns 00000011 
 */
byte get_least_bits(byte b, int index)
{
    int shift_val = 8 - (index + 1);	
    byte a = 255 >> shift_val, b1 = b & a;
    if(index < 0) return b;
    return b1;
}

/*
 * given a byte of the form 00000011 return 11000000
 *
byte format_byte(byte b, int shift_val)
{
    byte b1 = b << shift_val;
    return b1;
}
*/
void test(Buffer *buffer)
{
    int i, max = buffer->used;
    byte current;
    printf("testing buffer...\n");
    for(i = 0; i < max; i++)
    {
	current = buffer->data[i];
	printf("buffer->data[%d] = ", i); PRINT_BIN(current); printf(" = %d = %c\n", current, current);
    }
    printf("test done\n");
}

