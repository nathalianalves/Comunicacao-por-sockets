#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "tabuleiro_servidor.h"

#define TAM_TABULEIRO 8

// Cria e inicializa a estrutura do jogo
Jogo* criar_jogo() {
    Jogo* novo_jogo = malloc(sizeof(Jogo));
    if (!novo_jogo) return NULL;

    // Aloca matriz TAM_TABULEIROxTAM_TABULEIRO
    novo_jogo->tabuleiro = malloc(TAM_TABULEIRO * sizeof(Estado*));
    if (!novo_jogo->tabuleiro) {
        free(novo_jogo);
        return NULL;
    }

    for (int i = 0; i < TAM_TABULEIRO; i++) {
        novo_jogo->tabuleiro[i] = malloc(TAM_TABULEIRO * sizeof(Estado));
        if (!novo_jogo->tabuleiro[i]) {
            // Se falhar, libera o que já foi alocado
            for (int j = 0; j < i; j++) {
			    free(novo_jogo->tabuleiro[j]);
            }

            free(novo_jogo->tabuleiro);
            free(novo_jogo);
            return NULL;
        }
    }

    novo_jogo->jogador_x = 0;
    novo_jogo->jogador_y = 0;
    return novo_jogo;
}

// Libera a memória alocada
void destruir_jogo(Jogo* jogo) {
    if (!jogo) return;
    
    for (int i = 0; i < TAM_TABULEIRO; i++) {
        free(jogo->tabuleiro[i]);
    }
    free(jogo->tabuleiro);
    free(jogo);
}

// Inicializa o tabuleiro do jogo, colocando todas as posições como vazias e sorteando tesouros
void inicializar_tabuleiro(Jogo* jogo) {
    // Inicializa todas as posições como vazias
    for (int i = 0; i < TAM_TABULEIRO; i++) {
        for (int j = 0; j < TAM_TABULEIRO; j++) {
            jogo->tabuleiro[i][j] = VAZIO;
        }
    }

    // Sorteia 8 tesouros evitando a posição inicial (0,0)
    int tesouros = 0;
    while (tesouros < 8) {
        int x = rand() % TAM_TABULEIRO;
        int y = rand() % TAM_TABULEIRO;
        if ((jogo->tabuleiro[x][y] == VAZIO) && !(x == 0 && y == 0)) {
            jogo->tabuleiro[x][y] = TESOURO;
            tesouros++;
        }
    }
}

// Imprime o tabuleiro do jogo
void imprimir_tabuleiro(const Jogo* jogo) {
    printf("\n  +-------------------------------------------------+\n");
    
    for (int x = TAM_TABULEIRO - 1; x >= 0; x--) {
        printf("%d |", x);
        for (int y = 0; y < TAM_TABULEIRO; y++) {
            if (x == jogo->jogador_x && y == jogo->jogador_y) {
                printf(" P  ");
            } else {
                switch (jogo->tabuleiro[x][y]) {
                    case TESOURO:  printf(" $  "); break;
                    case VISITADO: printf(" X  "); break;
                    default:       printf(" .  ");
                }
            }
        }
        printf("|\n");
    }
    
    printf("  +-------------------------------------------------+\n");
    printf("     0   1   2   3   4   5   6   7\n");
}


