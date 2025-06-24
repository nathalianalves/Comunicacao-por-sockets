#ifndef _TABULEIRO_SERVIDOR_
#define _TABULEIRO_SERVIDOR_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TAM_TABULEIRO 8

typedef enum {
    VAZIO,
    TESOURO,
    VISITADO,
    JOGADOR
} Estado;

typedef struct {
    Estado** tabuleiro; // tabuleiro: matriz de estados
    int jogador_x;      // coordenada x (linha) do jogador
    int jogador_y;      // coordenada y (coluna) do jogador
} Jogo;

// Cria e inicializa a estrutura do jogo
Jogo* criar_jogo();

// Libera a memória alocada
void destruir_jogo(Jogo* jogo);

// Inicializa o tabuleiro do jogo, colocando todas as posições como vazias e sorteando tesouros
void inicializar_tabuleiro(Jogo* jogo);

// Imprime o tabuleiro do jogo
void imprimir_tabuleiro(const Jogo* jogo);

#endif