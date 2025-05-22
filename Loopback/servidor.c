#include "sockets.h"
#include <unistd.h>

int main() {
    // criamos um socket com interface loopback
    int sock = criaRawSocket("lo");
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

    close(sock);
    return 0;
}