.PHONY: clean
GCC = -gcc -Wall -Werror -pedantic -ggdb3 -std=gnu99 
SRCS = $(.wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

try: my_malloc.c
	$(GCC) -o $@ $<

clean:
	rm -f *.o *~ *#
