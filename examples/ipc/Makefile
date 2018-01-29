TARGETS=mmap_basic mmap_basic2 pipe_basic mmul
FLAGS =  -std=gnu99 -Wall -Werror -pedantic -pthread
all: $(TARGETS)

clean:
	rm -f *~ *# $(TARGETS)

mmap_basic: mmap_basic.c
	gcc -O3 $(FLAGS) -o $@ $< -lrt

mmap_basic2: mmap_basic2.c
	gcc -O3 $(FLAGS) -o $@ $< -lrt

pipe_basic: pipe_basic.c
	gcc -O3 $(FLAGS) -o $@ $<

mmul: mmul.cpp
	g++ -std=gnu++11 -Wall -Werror -pthread -pedantic -o $@ $<

thread: thread.cpp
	g++ -std=gnu++11  -pthread -Wall -Werror -pedantic -o $@ $<
