CC = gcc
CFLAGS = -Wall -Wextra
programaCliente = cliente 
programaServidor = servidor

all: cliente servidor

$(programaCliente): cliente.o sockets.o
	$(CC) -o $(programaCliente) cliente.o sockets.o $(CFLAGS)

$(programaServidor): servidor.o sockets.o
	$(CC)  -o $(programaServidor) servidor.o sockets.o $(CFLAGS)

sockets.o: sockets.c 
	gcc -c sockets.c $(CFLAGS)

cliente.o: cliente.c sockets.c
	$(CC) -c cliente.c sockets.c $(CFLAGS)

servidor.o: servidor.c sockets.c 
	$(CC) -c servidor.c sockets.c $(CFLAGS)

clean:
	rm -f *.o *.gch

purge:
	make clean
	rm -f $(programaCliente) $(programaServidor)