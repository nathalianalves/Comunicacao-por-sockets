#include "sockets.h"
#include <unistd.h>

int main() {
    // criamos um socket com interface loopback
    int sock = criaRawSocket("lo");
    unsigned char buffer[132];

    // cliente vai enviar mensagens
    while (1) {
        // coloca marcador de inicio
        buffer[0] = 0x7E;

        printf("Digite sua mensagem: ");
        fgets((char*)buffer+1, sizeof(buffer)-1, stdin);

        ssize_t envio = send(sock, buffer, 132, 0);
        if (envio >= 0) {
            printf("%ld bytes enviados\n", envio);
        }
        else {
            printf("Erro ao enviar mensagem\n");
        }
    }

    close(sock);
    return 0;
}