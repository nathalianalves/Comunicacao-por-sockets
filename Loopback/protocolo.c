// protocolo.c
#include "protocolo.h"
#include <string.h>

// Cria um frame válido
Frame criar_frame(uint8_t tipo, uint8_t sequencia, uint8_t* dados, uint8_t tamanho) {
    Frame f;
    f.marcador = MARCADOR_INICIO;
    f.tamanho = tamanho;
    f.sequencia = sequencia;
    f.tipo = tipo;
    if ((tamanho > 0) && (dados != NULL)) {
        memcpy(f.dados, dados, tamanho);
    }
    f.checksum = calcular_checksum(&f);

    printf("CRIANDO FRAME COM DADOS:\n");
    printf("marcador: %d\n", f.marcador);
    printf("tamanho: %d\n", f.tamanho);
    printf("sequencia: %d\n", f.sequencia);
    printf("tipo: %d\n", (f.tipo));
    printf("dados: %s\n", f.dados);

    return f;
}

// Transforma um frame em um vetor de dados
// Retorno: para sucesso, tamanho do buffer. Para erro, -1.
int serializar_frame(Frame* f, uint8_t* buffer, unsigned int tamanho_buffer) {
    f->checksum = calcular_checksum(f);
    
    int tamanho_total = TAM_CABECALHO + f->tamanho; 

    if (tamanho_buffer < (unsigned int) tamanho_total) {
        return -1;
    }
    
    memcpy(buffer, f, 4);
    if(f->tamanho > 0) {
        memcpy(buffer + 4, f->dados, f->tamanho);
    }
    
    return tamanho_total;
}

// Lê dados crus do buffer e preenche um frame. 
// Retorno: 0 para sucesso, -1 para erro de checksum.
int desserializar_frame(Frame* f, uint8_t* buffer, int tamanho_buffer) {
    uint8_t checksum_esperado;

    if (tamanho_buffer < 4) { 
        return -1;
    }
    
    memcpy(f, buffer, 4);

    if(tamanho_buffer < 4 + f->tamanho) {
        return -1;
    }

    memcpy(f->dados, buffer + 4, f->tamanho);

    checksum_esperado = calcular_checksum(f);
    if(f->checksum != checksum_esperado) {
        printf("[desserializar_frame] Erro no calculo de checksum\nRecebido:%d Calculado:%d\n", f->checksum, checksum_esperado);
        return -1;
    }
    
    return 0;
}

// Retorno: 1 se mensagem do buffer é válida no procolo, 0 se não é.
int protocolo_eh_valido(unsigned char* buffer, int tamanho_buffer) {
    if (tamanho_buffer < TAM_CABECALHO) {
        return 0; 
    }

    unsigned char marcador = buffer[0];

    unsigned char byte1 = buffer[1];
    unsigned char byte2 = buffer[2];

    unsigned char tamanho_dados = byte1 && 0x7F;

    if (marcador != MARCADOR_INICIO) {
        return 0;
    }

    if (tamanho_dados > TAM_MAX_DADOS) {
        return 0;
    }

    if (tamanho_buffer < (TAM_CABECALHO + tamanho_dados)) {
        return 0;
    }

    return 1; 
}

// Calcula o checksum dos campos: tamanho, sequencia, tipo e dados
uint8_t calcular_checksum(Frame* f) {
    uint8_t checksum = 0;

    checksum ^= f->tamanho;
    checksum ^= f->sequencia;
    checksum ^= f->tipo;
    for(int i = 0; i < f->tamanho; i++) {
        checksum ^= f->dados[i];
    }
    
    return checksum;
}

// Envia o frame frame_envio no socket ctx_socket
void enviar_frame(contexto_raw_socket ctx_socket, Frame frame_envio) {
    int enviados, tamanho;
    unsigned char buffer[sizeof(Frame)];

    tamanho = serializar_frame(&frame_envio, buffer, sizeof(buffer));

    printf("ENVIANDO FRAME COM DADOS:\n");
    printf("marcador: %d\n", frame_envio.marcador);
    printf("tamanho: %d\n", frame_envio.tamanho);
    printf("sequencia: %d\n", frame_envio.sequencia);
    printf("tipo: %d\n", (frame_envio.tipo));
    printf("dados: %s\n", frame_envio.dados);

    enviados = send(ctx_socket.socket_fd, buffer, sizeof(buffer), 0);
    if (enviados == -1) {
        perror("[enviar_frame] Erro no envio.\n");
        exit(-1);
    } else {
        if (frame_envio.tipo == TIPO_DADOS) {
            printf("Enviando tipo dados com buffer:\n");
            for (int i = 0; i < sizeof(Frame); i++) {
                printf("buffer[%d] = %d\n", i, buffer[i]);
            }
        } 
    }
}

int receber_mensagem(contexto_raw_socket ctx_socket, int timeoutMillis, unsigned char* buffer, int tamanho_buffer) {
    long long comeco = timestamp();
    struct timeval timeout = { .tv_sec = timeoutMillis/1000, .tv_usec = (timeoutMillis%1000) * 1000 };
    
    setsockopt(ctx_socket.socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout));
    int bytes_lidos;
    do {
        bytes_lidos = recv(ctx_socket.socket_fd, buffer, tamanho_buffer, 0);
        if (protocolo_eh_valido(buffer, bytes_lidos)) { 
            return bytes_lidos; 
        }
    } while (timestamp() - comeco <= timeoutMillis);

    return -1;
}

// Envia ACK 
// Retorno: 0 para falha, 1 para sucesso
/* int enviar_ack(contexto_raw_socket ctx_socket, char mac_origem[18], Frame f) {
    uint8_t vazio[] = {0};
    Frame ack = criar_frame(TIPO_ACK, f.sequencia, vazio, 0);

    // Enviar o ACK para o MAC de origem
    if (enviar_frame(ctx_socket, mac_origem, (unsigned char*)&ack, sizeof(Frame)) == -1) {
        printf("[enviar_ack] Erro em enviar_frame.\n");
        return 0;
    };

    printf("ACK enviado para %s\n", mac_origem);
    return 1;
}

int enviar_nack(contexto_raw_socket ctx_socket, char mac_origem[18], Frame f){
    uint8_t vazio[] = {0};
    Frame nack = criar_frame(TIPO_NACK, f.sequencia, vazio, 0);

    if (enviar_frame(ctx_socket, mac_origem, (unsigned char*)&nack, sizeof(Frame) == -1)) {
        printf("[enviar_nack] Erro em enviar_frame.\n");
        return 0;

    }
    
    printf("ACK enviado para %s\n", mac_origem);
    return 1;
} */