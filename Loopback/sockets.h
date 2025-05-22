#ifndef _SOCKETS_
#define _SOCKETS_

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <stdlib.h>
#include <stdio.h>

// Cria um raw socket e retorna seu descritor
int criaRawSocket(char* nomeInterface);

#endif