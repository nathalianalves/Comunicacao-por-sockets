// protocolo.h
#ifndef _PROTOCOLO_
#define _PROTOCOLO_

#include <stdint.h>

#define TAM_MAX_DADOS 127
#define MARCADOR_INICIO 0x7E  // 0111 1110

// Tipos de mensagem
enum TipoMensagem {
    TIPO_ACK = 0,
    TIPO_NACK = 1,
    TIPO_OK_ACK = 2,
    TIPO_LIVRE = 3,
    TIPO_TAMANHO = 4,
    TIPO_DADOS = 5,
    TIPO_TEXTO = 6,
    TIPO_VIDEO = 7,
    TIPO_IMAGEM = 8,
    TIPO_FIM_ARQUIVO = 9,
    TIPO_DIREITA = 10,
    TIPO_CIMA = 11,
    TIPO_BAIXO = 12,
    TIPO_ESQUERDA = 13,
    TIPO_LIVRE2 = 14,
    TIPO_ERRO = 15,
    TIPO_TABULEIRO =16
};

// Frame do protocolo
typedef struct {
    unsigned char marcador;      // marcador de inicio
    unsigned char tamanho : 7;   // 7 bits para tamanho 
    unsigned char sequencia : 5; // 5 bits para sequencialização
    unsigned char tipo : 4;      // 4 bits para tipo
    unsigned char checksum;      // 8 bits
    unsigned char dados[];       // até 127 bytes
} Frame;

// Funções
// Cria um frame válido com os dados passados por parâmetro
Frame empacotar(uint8_t tipo, uint8_t sequencia, uint8_t* dados, uint8_t tamanho);

// Lê dados crus do buffer e preenche um frame. Retorna 0 para sucesso, -1 para erro.
int desempacotar(Frame* f, const uint8_t* buffer, int buflen);

// Calcula o checksum dos campos tamanho, sequencia, tipo e dados
uint8_t calcular_checksum(const Frame* f);

#endif
