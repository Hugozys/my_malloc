CC=gcc
CFLAGS= -O3 -fPIC
DEPS=my_malloc.h
WE = -Wall -Werror -pedantic
#DEBUG = -ggdb3
all: lib

lib: my_malloc.o
	$(CC) $(CFLAGS) -shared -o libmymalloc.so my_malloc.o

%.o: %.c my_malloc.h
	$(CC) $(CFLAGS) $(WE)  -c -o $@ $< 

clean:
	rm -f *~ *.o *.so

clobber:
	rm -f *~ *.o
