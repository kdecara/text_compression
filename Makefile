
CC = gcc
CFLAGS = -Wall -g -std=c89 -pedantic-errors

TARGET = huffman_text_compression

all: $(TARGET)

$(TARGET): $(TARGET).c file_pack.c dynamic_array.c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c file_pack.c dynamic_array.c

clean:
	rm $(TARGET)

test: $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c
	./huffman_text_compression tfile.txt packed.out
	ls -l tfile.txt
	ls -l packed.out

val:
	valgrind --leak-check=yes ./text_compression compress tfile.txt tbinary.bin
