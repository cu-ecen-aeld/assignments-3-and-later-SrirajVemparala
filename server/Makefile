all: aesdsocket

CROSS_COMPILE=
CC ?= $(CROSS_COMPILE)gcc

FLAGS ?= -Wall -Werror
LDFLAGS ?= -pthread -lrt

aesdsocket:
	$(CC) $(FLAGS) $(LDFLAGS) -o aesdsocket aesdsocket.c

clean: 
	rm -f *.o aesdsocket
