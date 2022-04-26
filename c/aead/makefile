CC=gcc
IDIR=./inc
CFLAGS=-I$(IDIR) -Wall

DEPS= test.h xoodoo.h xoodyak.h
OBJ= main.o test.o xoodoo.o xoodyak.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

hellomake: $(OBJ)
	$(CC) -o main.out $^ $(CFLAGS)
	rm $(OBJ)	

egypt: $(OBJ)
	$(CC) $(IMG) -o main.rlt.out $^ $(CFLAGS)
	rm $(OBJ)	