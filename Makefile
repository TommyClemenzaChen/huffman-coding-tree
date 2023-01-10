CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic -gdwarf-4

all: encode decode

decode: node.o decode.o pq.o code.o huffman.o io.o stack.o
	$(CC) -o $@ $^
encode: node.o pq.o code.o huffman.o io.o stack.o encode.o
	$(CC) -o $@ $^
%.o: %.c
	$(CC) $(CFLAGS) -c $<

format:
	clang-format -i -style=file code.c decode.c encode.c huffman.c io.c node.c pq.c


clean:
	rm -f *.o
spotless:
	rm -f $(EXECBIN) *.o
 
