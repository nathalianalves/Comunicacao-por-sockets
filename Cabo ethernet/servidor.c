#include <stdio.h>
#include <string.h>
#include "protocolo.h"

extern int cria_raw_socket(const char* interface);
extern int recebe(int socket, unsigned char* dados, char* origem);
extern int envia(int socket, const char* destino, const unsigned char* dados, int tamanho);

int main() {
    int sock = cria_raw_socket("enp1s0");  // ajuste para sua interface
    if (sock < 0) {
        perror("Erro ao criar raw socket");
        return 1;
    }

    unsigned char buffer[sizeof(Frame)];
    char mac_origem[18];

    while (1) {
        int lidos = recebe(sock, buffer, mac_origem);
        if (lidos > 0) {
            Frame f;
            if (buffer[0] != 0x7E) continue;
            if (desempacotar(&f, buffer, lidos) == 0) {
                printf("Recebido tipo: %d de %s\n", f.tipo, mac_origem);

                // Enviar ACK de volta
                uint8_t dados_dummy[] = {0};
                Frame ack = empacotar(TIPO_ACK, f.sequencia, dados_dummy, 0);
                envia(sock, mac_origem, (unsigned char*)&ack, sizeof(Frame));
                printf("ACK enviado\n");
            } else {
                printf("Erro ao desempacotar frame.\n");
            }
        }
    }

    return 0;
}
