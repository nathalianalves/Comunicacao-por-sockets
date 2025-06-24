#include "cliente.h"

// Loop infinito para ler teclas e verificar validade
char ler_tecla_valida() {
    char c;

    while (1) {
        printf("Entre com uma tecla (w/a/s/d): ");
        if (scanf(" %c", &c) == 1) {
            if ((c == 'w') || (c == 'a') || (c == 's') || (c == 'd')) {
                return c;
            } else {
                printf("Tecla inválida! Use apenas w/a/s/d.\n");
                while (getchar() != '\n');
            }
        }
    }
}

// Retorna o espaço disponivel em caminho
unsigned long long obter_espaco_disponivel(char *caminho) {
    struct statvfs svfs;

    if (statvfs(caminho, &svfs) != 0) {
        perror(caminho);//"[obter_espaco_disponivel] Erro em statvfs %s=");
        return 0;
    }

    return svfs.f_bavail * svfs.f_frsize;
}

// A partir dos dados de frame, extrai o nome do arquivo 
void extrair_nome_arquivo(Frame f, unsigned char* nome_arquivo) {
    printf("Nome arquivo tem %d caracteres\n", (int)f.tamanho);

    for (int i = 0; i < (int)f.tamanho; i++) {
        nome_arquivo[i] = f.dados[i];
    }
}

void gerar_caminho_completo(char* caminho_completo, char* caminho_diretorio, char* nome_arquivo) {
    strcpy(caminho_completo, caminho_diretorio);
    strcat(caminho_completo, "/");
    strcat(caminho_completo, nome_arquivo);
}

void processar_frame_tamanho(Frame* f, uint32_t* tamanho) {
    uint32_t tamanho_no_frame;
    memcpy(&tamanho_no_frame, f->dados, sizeof(tamanho_no_frame));
    
    *tamanho = ntohl(tamanho_no_frame);    
}