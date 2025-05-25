CC = gcc
CFLAGS = -Wall -Wextra
programaCliente = cliente 
programaServidor = servidor

all: $(programaCliente) $(programaServidor)

#$(programaCliente): mainCliente.o cliente.o sockets.o
#	$(CC) -o $(programaCliente) cliente.o sockets.o $(CFLAGS)

$(programaServidor): mainServidor.o servidor.o sockets.o
	$(CC) -o $(programaServidor) mainServidor.o servidor.o sockets.o $(CFLAGS)

mainServidor.o: mainServidor.c servidor.c sockets.c
	gcc -c mainServidor.c servidor.c sockets.c $(CFLAGS)

servidor.o: servidor.c
	gcc -c servidor.c $(CFLAGS)

sockets.o: sockets.c 
	gcc -c sockets.c $(CFLAGS)

clean:
	rm -f *.o *.gch

purge:
	make clean
	rm -f $(programaCliente) $(programaServidor)