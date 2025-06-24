#include "cliente.h"

// Imprime o tabuleiro do jogo
// . se está vazio, X se já foi visitado, P se é a posição atual do jogador
int mostra_tabuleiro (char* buffer, Frame f){
    printf("\nTABULEIRO INICIAL (sem tesouros):\n\n");
    for (int i = TAM_TABULEIRO - 1; i >= 0; i--) {
        for (int j = 0; j < TAM_TABULEIRO; j++) {
            uint8_t valor = f.dados[i * TAM_TABULEIRO + j];
            switch (valor) {
                case VAZIO:    printf(" . "); break;
                case VISITADO: printf(" X "); break;
                case JOGADOR:  printf(" P "); break;
                default:       printf(" ? "); break;
            }
        }
        printf("\n");
    }
}

// Envia o movimento solicitado pelo cliente para o servidor
void envia_movimento(char teclaescolhida, Frame f, int sock, char mac_origem[18]){
    uint8_t tipo_movimento;

    switch (teclaescolhida) {
        case 'w': tipo_movimento = TIPO_CIMA; break;
        case 's': tipo_movimento = TIPO_BAIXO; break;
        case 'd': tipo_movimento = TIPO_DIREITA; break;
        case 'a': tipo_movimento = TIPO_ESQUERDA; break;
        default:
            printf("Tecla inválida no envio.\n");
            return;
    }

    uint8_t vazio[] = {0};
    Frame movimento = empacotar(tipo_movimento, f.sequencia + 1, vazio, 0);
    envia_cliente(sock, mac_origem, (unsigned char*)&movimento, sizeof(Frame));
    printf("Movimento enviado (%d).\n", tipo_movimento);
}

// Envia ACK para servidor
int confirma_que_recebeu(int sock, char mac_origem[18], Frame f){
    uint8_t vazio[] = {0};
    Frame ack = empacotar(TIPO_ACK, f.sequencia, vazio, 0);

    // Enviar o ACK de volta para o MAC de origem
    envia_cliente(sock, mac_origem, (unsigned char*)&ack, sizeof(Frame));
    printf("ACK enviado para %s\n", mac_origem);
}

// Loop infinito para ler teclas e verificar validade
char ler_tecla_valida() {
    char c;
    while (1) {
        printf("Digite uma tecla (w/a/s/d): ");
        if (scanf(" %c", &c) == 1) {
            if (c == 'w' || c == 'a' || c == 's' || c == 'd') {
                return c;
            } else {
                printf("Tecla inválida! Use apenas w/a/s/d.\n");
                // limpa qualquer caractere extra, inclusive '\n'
                while (getchar() != '\n');
            }
        }
    }
}