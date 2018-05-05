CFLAGS=-std=gnu11 -O2 -Wall -Wno-unused-function
LDFLAGS=-lpthread

all: collide

.PHONY: run
run: collide
	./collide

collide: brute_search.o constraint_search.o collide.o curl.o converter.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.c *.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o collide
