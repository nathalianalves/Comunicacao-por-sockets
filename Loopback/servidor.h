/* Biblioteca de funções relacionadas ao servidor da caça ao tesouro
*
* Explicação de estruturas:
* - Grid: mapa do jogo, representado por uma matriz
* - Tesouros: as posições dos tesouros são sorteadas e armazenadas fora do grid principal, em uma matriz com duas colunas na qual matriz[i][0] e matriz[i][1] são, respectivamente, a linha e a coluna do tesouro i no grid */

#ifndef _SERVIDOR_
#define _SERVIDOR_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LINHAS_GRID 8
#define COLUNAS_GRID 8
#define NUM_TESOUROS 8

// Inicializa o grid usado como mapa no jogo
int **inicializaGrid();

// Apaga o grid usado como mapa no jogo
void desalocaGrid(int **grid);

// Sorteia as posições dos tesouros do jogo
int** sorteiaTesouros();

// Desaloca a matriz que armazena a posição dos tesouros
void desalocaTesouros(int** tesouros);

#endif