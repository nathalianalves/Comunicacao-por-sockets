#include "servidor.h"

/*------------------------------ FUNÇÕES AUXILIARES ------------------------------*/
// Aloca uma matriz linhas x colunas
int** alocaMatriz(int linhas, int colunas) {
	int** matriz;

	matriz = (int**)malloc(sizeof(int*)*linhas);
	if (!matriz) {
		printf("ERRO: malloc falhou.\n");
		exit(-1);
	}

	for (int i = 0; i < linhas; i++) {
		matriz[i] = (int*) malloc(sizeof(int) * colunas);
		if (!matriz[i]) {
			printf("ERRO: malloc falhou.\n");
			exit(-1);
		}
	}

	return matriz;
}

// Desaloca uma matriz 
// Parâmetros:
// - matriz: matriz desalocada
// - linhas: número de linhas da matriz 
void desalocaMatriz(int** matriz, int linhas) {
	for (int i = 0; i < linhas; i++) {
		free(matriz[i]);
	}
	free(matriz);
}
/*------------------------------ FIM DAS FUNÇÕES AUXILIARES ------------------------------*/

/*------------------------------ FUNÇÕES PRINCIPAIS ------------------------------*/
// Inicializa o mapa do jogo 
// Se grid[i][j]:
// - é -1: jogador não passou por grid[i][j]
// - é 0: jogador está em grid[i][j]
// - é 1: jogador já passou por grid[i][j] e encontrou tesouro
// - é 2: jogador já passou por grid[i][j] e não encontrou nada 
int **inicializaGrid() {
	int **grid;

	grid = alocaMatriz(LINHAS_GRID, COLUNAS_GRID);

	for (int i = 0; i < LINHAS_GRID; i++) {
		for (int j = 0; j < COLUNAS_GRID; j++) {
			grid[i][j] = -1;
		}
	}
	grid[0][0] = 0;

	return grid;
}

// Apaga o grid usado como mapa no jogo
void desalocaGrid(int **grid) {
	desalocaMatriz(grid, LINHAS_GRID);
}

// Sorteia as posições dos tesouros do jogo
int** sorteiaTesouros() {
	int **tesouros;

	/* A linha abaixo está comentada para facilitar testes, já que, com ela, as posições dos tesouros são mudam a cada execução*/
	//srand(time(NULL));
	
	// Aloca a matriz de tesouros
	tesouros = alocaMatriz(NUM_TESOUROS, 2);
	
	// Sorteia linha e coluna de cada um dos tesouros
	for (int i = 0; i < NUM_TESOUROS; i++) {
		tesouros[i][0] = rand() % LINHAS_GRID;
		tesouros[i][1] = rand() % COLUNAS_GRID;
	}

	return tesouros;
}

// Desaloca a matriz que armazena a posição dos tesouros
void desalocaTesouros(int** tesouros) {
	desalocaMatriz(tesouros, NUM_TESOUROS);
}