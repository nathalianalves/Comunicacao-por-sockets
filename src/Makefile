CC = gcc
CFLAGS = -Wall -Wextra -lm
programa_cliente = cliente 
programa_servidor = servidor

all: $(programa_cliente) $(programa_servidor)

$(programa_cliente): main_cliente.o sockets.o protocolo.o timestamp.o tabuleiro.o cliente.o
	$(CC) -o $(programa_cliente) main_cliente.o sockets.o protocolo.o timestamp.o tabuleiro.o cliente.o $(CFLAGS)

$(programa_servidor): main_servidor.o sockets.o protocolo.o tabuleiro.o timestamp.o servidor.o
	$(CC) -o $(programa_servidor) main_servidor.o sockets.o protocolo.o tabuleiro.o timestamp.o servidor.o $(CFLAGS)

clean:
	rm -f *.o *.gch

purge:
	make clean
	rm -f $(programa_cliente) $(programa_servidor)