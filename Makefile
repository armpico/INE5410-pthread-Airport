## Makefile da Simulacao de aeroporto.
CC=gcc
CFLAGS=-Wall -g -I. -lpthread -std=gnu11
EXEC=simulacao

all: $(EXEC)

%.o: %.c
	$(CC) -c $^ $(CFLAGS)

$(EXEC): main.o aeroporto.o aviao.o fila.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm *.o $(EXEC)
