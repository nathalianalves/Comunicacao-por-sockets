#ifndef _CLIENTE_
#define _CLIENTE_

#include "protocolo.h"
#include "tabuleiro_servidor.h"
#include "funcoesfornecidass.h"

// Imprime o tabuleiro do jogo
// . se está vazio, X se já foi visitado, P se é a posição atual do jogador
int mostra_tabuleiro (char* buffer, Frame f);

// Envia o movimento solicitado pelo cliente para o servidor
void envia_movimento(char teclaescolhida, Frame f, int sock, char mac_origem[18]);

// Envia ACK para servidor
int confirma_que_recebeu(int sock, char mac_origem[18], Frame f);

// Loop infinito para ler teclas e verificar validade
char ler_tecla_valida();

#endif