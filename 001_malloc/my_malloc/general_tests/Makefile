CC=gcc
CFLAGS= -O3 -fPIC
MALLOC_VERSION=BF
WDIR=..

all: mymalloc_test my_test

mymalloc_test: mymalloc_test.c
	$(CC)  $(CFLAGS) -I$(WDIR) -L$(WDIR) -D$(MALLOC_VERSION) -Wl,-rpath=$(WDIR) -o $@ mymalloc_test.c -lmymalloc -lrt

my_test: customized_mymalloc_test.c
	$(CC) -ggdb3 $(CFLAGS) -I$(WDIR) -L$(WDIR) -D$(MALLOC_VERSION) -Wl,-rpath=$(WDIR) -o $@ customized_mymalloc_test.c -lmymalloc -lrt
clean:
	rm -f *~ *.o mymalloc_test

clobber:
	rm -f *~ *.o
