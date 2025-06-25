#ifndef _TABULEIRO_
#define _TABULEIRO_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define TAM_TABULEIRO 8
#define NUM_TESOUROS 8

typedef enum {
    VAZIO,
    TESOURO,
    VISITADO,
    JOGADOR
} Estado;

// Imprime o tabuleiro do jogo
void imprimir_tabuleiro(uint8_t* tabuleiro);

#endif