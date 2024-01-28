COMPILER ?= $(CROSS_COMPILER)gcc

FLAGS = -Wall -Werror

.PHONY: all
all: writer

writer: writer_o
	$(COMPILER) $(FLAGS) writer.o -o writer

writer_o:
	$(COMPILER) $(FLAGS) -c writer.c -o writer.o

.PHONY: clean
clean: 
	rm -f *.o writer
