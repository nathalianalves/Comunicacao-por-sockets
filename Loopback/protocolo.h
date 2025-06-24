// protocolo.h
#ifndef _PROTOCOLO_
#define _PROTOCOLO_

#include "sockets.h"
#include <stdint.h>

#define TAM_MAX_DADOS 127
#define MARCADOR_INICIO 0x7E  
#define TIMEOUT_MILI 5000
#define TAM_CABECALHO 4

// Tipos de mensagem
enum TipoMensagem {
    TIPO_ACK,
    TIPO_NACK,
    TIPO_OK_ACK,
    TIPO_TABULEIRO,
    TIPO_TAMANHO,
    TIPO_DADOS,
    TIPO_TEXTO,
    TIPO_VIDEO,
    TIPO_IMAGEM,
    TIPO_FIM_ARQUIVO,
    TIPO_DIREITA,
    TIPO_CIMA,
    TIPO_BAIXO,
    TIPO_ESQUERDA,
    TIPO_LIVRE2,
    TIPO_ERRO
};

// Frame do protocolo
typedef struct {
    unsigned char marcador;      
    unsigned char tamanho : 7;    
    unsigned char sequencia : 5; 
    unsigned char tipo : 4;      
    unsigned char checksum;                   
    unsigned char dados[TAM_MAX_DADOS];       
} __attribute__((packed)) Frame;

// Cria um frame válido com os dados passados por parâmetro
Frame criar_frame(uint8_t tipo, uint8_t sequencia, uint8_t* dados, uint8_t tamanho);

// Transforma um frame em um vetor de dados
// Retorno: para sucesso, tamanho do buffer. Para erro, -1.
int serializar_frame(Frame* f, uint8_t* buffer, unsigned int tamanho_buffer);

// Lê dados crus do buffer e preenche um frame. 
// Retorno: 0 para sucesso, -1 para erro.
int desserializar_frame(Frame* f, uint8_t* buffer, int tamanho_buffer);

// Retorno: 1 se mensagem do buffer é válida no procolo, 0 se não é.
int protocolo_eh_valido(unsigned char* buffer, int tamanho_buffer);

// Calcula o checksum dos campos tamanho, sequencia, tipo e dados
uint8_t calcular_checksum(Frame* f);

// Envia o frame frame_envio no socket ctx_socket
void enviar_frame(contexto_raw_socket ctx_socket, Frame frame_envio);

// Retorno: numero de bytes lidos ou, se ocorrer timeout, -1
int receber_mensagem(contexto_raw_socket ctx_socket, int timeoutMillis, unsigned char* buffer, int tamanho_buffer);

#endif
