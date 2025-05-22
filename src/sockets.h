#ifndef _SOCKETS_
#define _SOCKETS_

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>

/* Cria um raw socket e retorna seu descritor de arquivo
 * Parâmetros:
 * - ifname: nome da interface de rede */
int criaRawSocket(char *ifname);

int envia(int socket, const char* destino_mac_str, unsigned char *interface, const unsigned char* dados, int tamanho) {

int recebe(int socket, unsigned char* dados, char* origem_mac_str) {

#endif