CC = gcc
CFLAGS = -Wall -g
 
all: ht-comm
 
ht-comm: ht_comm.o hash.o ht.o
	$(CC) $^ -o $@
 
ht_comm.o: ht_comm.c ht.h hash.h
	$(CC) $(CFLAGS) -c $^
 
hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c $^

ht.o: ht.c ht.h
	$(CC) $(CFLAGS) -c $^


.PHONY: clean
clean:
	rm -f *.o *~ tema
