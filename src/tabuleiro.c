#include "tabuleiro.h"

// Imprime o tabuleiro do jogo
void imprimir_tabuleiro(uint8_t* tabuleiro) {
    //system("clear");
    
    printf("____________________________________\n");
    for (int i = TAM_TABULEIRO - 1; i >= 0; i--) {
        printf("%d |", i);
        for (int j = 0; j < TAM_TABULEIRO; j++) {
            switch(tabuleiro[i * TAM_TABULEIRO + j]) {
                case VAZIO:
                    printf(" .  ");
                    break;

                case TESOURO:
                    printf(" $  "); 
                    break;

                case VISITADO: 
                    printf(" X  "); 
                    break;
                
                case JOGADOR:
                    printf(" P  ");
                    break;   
                
                default:
                    printf(" ?  ");
                    break;
            }
        }
        printf("|\n");
    }

    printf("____________________________________\n");
    printf("     0   1   2   3   4   5   6   7\n");
}

