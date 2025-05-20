#include <stdio.h>
#include <string.h>
#include "protocolo.h"

extern int cria_raw_socket(const char* interface);
extern int envia(int socket, const char* destino, const unsigned char* dados, int tamanho);

int main() {
    int sock = cria_raw_socket("enx00e04c2807e3");  // ajuste para sua interface
    if (sock < 0) {
        perror("Erro ao criar raw socket");
        return 1;
    }

    uint8_t dados_dummy[] = {0}; // sem dados
    Frame f = empacotar(TIPO_CIMA, 0, dados_dummy, 0);

    char mac_destino[] = "88:6f:d4:fc:af:14";// MAC do servidor (ajuste para o real)

    if (envia(sock, mac_destino, (unsigned char*)&f, sizeof(Frame)) < 0) {
        perror("Erro ao enviar frame");
    } else {
        printf("Movimento enviado (cima).\n");
    }

    return 0;
}


