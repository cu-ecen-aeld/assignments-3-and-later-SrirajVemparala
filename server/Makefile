CC = $(CROSS_COMPILE)gcc

FLAGS = -Wall -Werror

.PHONY: all
all: aesdsocket

aesdsocket: socket
	$(CC) $(FLAGS) aesdsocket.o -o aesdsocket

socket:
	$(CC) $(FLAGS) -c aesdsocket.c -o aesdsocket.o

.PHONY: clean
clean: 
	rm -f *.o aesdsocket
