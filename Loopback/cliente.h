#ifndef _CLIENTE_
#define _CLIENTE_

#include <stdio.h>
#include <sys/statvfs.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "sockets.h"
#include "protocolo.h"
#include "tabuleiro.h"
#include "funcoesfornecidass.h"

// Loop infinito para ler teclas e verificar validade
char ler_tecla_valida();

// Retorna o espaço disponivel em caminho
unsigned long long obter_espaco_disponivel(char *caminho);

// A partir dos dados de frame, extrai o nome do arquivo 
void extrair_nome_arquivo(Frame f, unsigned char* nome_arquivo);

// Concatena as strings, gerando o caminho completo de nome_arquivo
void gerar_caminho_completo(char* caminho_completo, char* caminho_diretorio, char* nome_arquivo);

void processar_frame_tamanho(Frame* f, uint32_t* tamanho);
#endif