#include "servidor.h"

// Inicializa o tabuleiro do jogo, colocando todas as posições como vazias e sorteando tesouros
void inicializar_tabuleiro(Jogo* jogo) {
    for (int i = 0; i < TAM_TABULEIRO * TAM_TABULEIRO; i++) {
        jogo->tabuleiro[i] = VAZIO;
    }

    int num_tesouros_sorteados = 0;
    while (num_tesouros_sorteados < NUM_TESOUROS) {
        int i = rand() % TAM_TABULEIRO;
        int j = rand() % TAM_TABULEIRO;
        if ((jogo->tabuleiro[i * TAM_TABULEIRO + j] == VAZIO) && (i != 0) && (j != 0)) {
            jogo->tabuleiro[i * TAM_TABULEIRO + j] = TESOURO;
            jogo->tesouros[num_tesouros_sorteados] = i * TAM_TABULEIRO + j;
            num_tesouros_sorteados++;
        }
    }

    jogo->tabuleiro[jogo->jogador_linha * TAM_TABULEIRO + jogo->jogador_coluna] = JOGADOR;
}

// Cria e inicializa a estrutura do jogo
Jogo* criar_jogo() {
    Jogo* novo_jogo = (Jogo*) malloc(sizeof(Jogo));
    if (!novo_jogo) {
        printf("[criar_jogo] Erro em malloc.\n");
        return NULL;
    }

    novo_jogo->jogador_linha = 0;
    novo_jogo->jogador_coluna = 0;

    inicializar_tabuleiro(novo_jogo);

    return novo_jogo;
}

// Libera a memória alocada
void destruir_jogo(Jogo* jogo) {
    if (!jogo) {
        return;
    }

    free(jogo->tabuleiro);
    free(jogo);
}

// Transforma todos os campos TESOURO em VAZIO 
void esconder_tesouros(Jogo* jogo, uint8_t* dados_tabuleiro) {
    for (int i = 0; i < TAM_TABULEIRO*TAM_TABULEIRO; i++) {
        if (jogo->tabuleiro[i] == TESOURO) {
            dados_tabuleiro[i] = VAZIO; 
        } else {
            dados_tabuleiro[i] = jogo->tabuleiro[i];
        }
    }
}

// Verifica se o movimento pode ser feito. 
// Retorno: 1 se é possivel fazer o movimento, 0 caso contrario
int verificar_movimentacao(Jogo* jogo, int movimento) {
    switch(movimento) {
        // Direita
        case 0:
            if (jogo->jogador_coluna + 1 >= TAM_TABULEIRO) {
                return 0;
            }
            return 1;
            break;
        
        // Cima
        case 1:
            if (jogo->jogador_linha + 1 >= TAM_TABULEIRO) {
                return 0;
            }
            return 1;
            break;
        
        // Baixo
        case 2:
            if (jogo->jogador_linha - 1 < 0) {
                return 0;
            }
            return 1;
            break;

        // Esquerda
        case 3: 
            if (jogo->jogador_coluna - 1 < 0) {
                return 0;
            }
            return 1;
            break;
    }

    return -1;
}

// Efetua o movimento passado como parâmetro
void efetuar_movimentacao(Jogo* jogo, int movimento, int* tesouro_encontrado) {
    (*tesouro_encontrado) = 0;

    jogo->tabuleiro[jogo->jogador_linha * TAM_TABULEIRO + jogo->jogador_coluna] = VISITADO;
    
    switch (movimento) {
        // Direita
        case 0:
            jogo->jogador_coluna++;
            break;

        // Cima
        case 1:
            jogo->jogador_linha++;
            break;
        
        // Baixo
        case 2:
            jogo->jogador_linha--;
            break;
        
        // Esquerda
        case 3: 
            jogo->jogador_coluna--;
            break;
    }

    if (jogo->tabuleiro[jogo->jogador_linha * TAM_TABULEIRO + jogo->jogador_coluna] == TESOURO) {
        (*tesouro_encontrado) = 1;
    }

    jogo->tabuleiro[jogo->jogador_linha * TAM_TABULEIRO + jogo->jogador_coluna] = JOGADOR;
}

// Zera o vetor dados
void zerar_dados(uint8_t* dados, int tamanho_dados) {
    for (int i = 0; i < tamanho_dados; i++) {
        dados[i] = 0;
    }
}

void obter_caminho_tesouro(int num_tesouro, char* caminho_tesouro) {
    switch(num_tesouro) {
        case 1:
            strncpy(caminho_tesouro, TESOURO_1, TAM_MAX_DADOS);
            break;

        case 2:
            strncpy(caminho_tesouro, TESOURO_2, TAM_MAX_DADOS);
            break;

        case 3:
            strncpy(caminho_tesouro, TESOURO_3, TAM_MAX_DADOS);
            break;

        case 4:
            strncpy(caminho_tesouro, TESOURO_4, TAM_MAX_DADOS);
            break;

        case 5:
            strncpy(caminho_tesouro, TESOURO_5, TAM_MAX_DADOS);
            break;

        case 6:
            strncpy(caminho_tesouro, TESOURO_6, TAM_MAX_DADOS);
            break;
        
        case 7:
            strncpy(caminho_tesouro, TESOURO_7, TAM_MAX_DADOS);
            break;

        case 8:
            strncpy(caminho_tesouro, TESOURO_8, TAM_MAX_DADOS);
            break;
    }
}

// A partir do caminho do tesouro, extrai o nome do arquivo
void obter_nome_tesouro(char* nome_tesouro, char* caminho_tesouro) {
    char* separador = strrchr(caminho_tesouro, '/');

    if (separador) {
        strcpy(nome_tesouro, separador+1);
    } else {
        strcpy(nome_tesouro, caminho_tesouro);
    }
}

// Retorno: frame tamanho com atributos do tesouro num_tesouro
Frame criar_frame_tamanho(int num_tesouro, char* nome_tesouro, uint32_t* tamanho_tesouro) {
    uint8_t dados[4];  

    obter_caminho_tesouro(num_tesouro, nome_tesouro);

    struct stat st;
    if (stat(nome_tesouro, &st) != 0) {
        perror("[criar_frame_tamanho] Erro no stat");
        exit(-1);
    }

    *tamanho_tesouro = st.st_size;

    uint32_t tamanho_para_frame = htonl(*tamanho_tesouro);
    memcpy(dados, &tamanho_para_frame, sizeof(uint32_t));

    return criar_frame(TIPO_TAMANHO, 0, dados, sizeof(uint32_t));
}

// Extrai o tipo de arquivo do tesouro
// Retorno:
//  - se nome_tesouro é um texto, retorna 0
//  - se nome_tesouro é uma imagem, retorna 1
//  - se nome_tesouro é um video, retorna 2
int extrair_tipo_tesouro(char* nome_tesouro) {
    if (strstr(nome_tesouro, ".txt"))
        return 0;
    
    if (strstr(nome_tesouro, ".jpg"))
        return 1;

    if (strstr(nome_tesouro, ".mp4"))
        return 2;

    return -1;
}