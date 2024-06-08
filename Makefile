CC = gcc 
CFLAGS +=-g -O3

targets = knap knap1
objects = knap.o knap1.o

.PHONY : default
default : all

.PHONY : all
all : $(targets)

knap : knap.c
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

knap1 : knap1.c
	$(CC) $(CFLAGS) -fopenmp -o $@ $^

.PHONY : clean
clean:
	rm -f $(targets) $(objects) *~ 