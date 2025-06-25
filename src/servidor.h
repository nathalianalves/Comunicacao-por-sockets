#ifndef _SERVIDOR_
#define _SERVIDOR_

#include <stdio.h>
#include <stdint.h>
#include <sys/stat.h>
#include "tabuleiro.h"
#include "protocolo.h"

#define TESOURO_1 "objetos/1.mp4"
#define TESOURO_2 "objetos/2.txt"
#define TESOURO_3 "objetos/3.txt"
#define TESOURO_4 "objetos/4.jpg"
#define TESOURO_5 "objetos/5.txt"
#define TESOURO_6 "objetos/6.jpg"
#define TESOURO_7 "objetos/7.jpg"
#define TESOURO_8 "objetos/8.txt"

typedef struct {
    uint8_t tabuleiro[TAM_TABULEIRO * TAM_TABULEIRO];  
    uint8_t tesouros[NUM_TESOUROS];
    int jogador_linha;  
    int jogador_coluna; 
} Jogo;

// Cria e inicializa a estrutura do jogo
Jogo* criar_jogo();

// Libera a memória alocada
void destruir_jogo(Jogo* jogo);

// Transforma todos os campos TESOURO em VAZIO 
void esconder_tesouros(Jogo* jogo, uint8_t* dados_tabuleiro);

// Verifica se o movimento pode ser feito. 
// Uso do parâmetro movimento:
//  - Se 0, verifica movimento para direita
//  - Se 1, verifica movimento para cima
//  - Se 2, verifica movimento para baixo
//  - Se 3, verifica movimento para esquerda
// Retorno: 1 se é possivel fazer o movimento, 0 caso contrario
int verificar_movimentacao(Jogo* jogo, int movimento);

// Efetua o movimento passado como parâmetro
// Uso do parâmetro movimento:
//  - Se 0, efetua movimento para direita
//  - Se 1, efetua movimento para cima
//  - Se 2, efetua movimento para baixo
//  - Se 3, efetua movimento para esquerda
void efetuar_movimentacao(Jogo* jogo, int movimento, int* tesouro_encontrado);

// Zera o vetor dados
void zerar_dados(uint8_t* dados, int tamanho_dados);

// Retorno: nome do tesouro com numero num_tesouro+1
void obter_caminho_tesouro(int num_tesouro, char* caminho_tesouro);

// A partir do caminho do tesouro, extrai o nome do arquivo
void obter_nome_tesouro(char* nome_tesouro, char* caminho_tesouro);

// Retorno: frame tamanho com atributos do tesouro num_tesouro
Frame criar_frame_tamanho(int num_tesouro, char* nome_tesouro, uint32_t* tamanho_tesouro);

// Extrai o tipo de arquivo do tesouro
// Retorno:
//  - se nome_tesouro é um texto, retorna 0
//  - se nome_tesouro é uma imagem, retorna 1
//  - se nome_tesouro é um video, retorna 2
int extrair_tipo_tesouro(char* nome_tesouro);

#endif