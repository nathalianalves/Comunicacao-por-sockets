#include "sockets.h"
#include "servidor.h"
#include <unistd.h>

int main() {
	int **mapa;

	mapa = inicializaGrid();
	for (int i = 0; i < LINHAS_GRID; i++) {
		for (int j = 0; j < COLUNAS_GRID; j++) {
			printf("%2d ", mapa[i][j]);
		}
		printf("\n");
	}

	desalocaGrid(mapa);

	int** tesouros = sorteiaTesouros();
	for (int i = 0; i < NUM_TESOUROS; i++) {
		printf("Tesouro %d: %d %d\n", i, tesouros[i][0], tesouros[i][1]);
	}
	
  // criamos um socket com interface loopback
  /*int sock = criaRawSocket("lo");
  unsigned char buffer[132];

  // vamos fazer um loop para esperar mensagens
  while (1) {
  	// recebe dados
    ssize_t tam = recv(sock, buffer, 132, 0);
    if (tam > 0) {
      // verifica marcador de inicio
      if (buffer[0] == 0x7E) {
        printf("%ld bytes foram recebidos\n", tam);
        printf("\tMensagem: %s\n", buffer+1);
      }
    }
  }
  close(sock);*/
  
	return 0;
}