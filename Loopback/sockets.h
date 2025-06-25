#ifndef _SOCKETS_
#define _SOCKETS_

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "timestamp.h"

typedef struct {
	int socket_fd;
    int ifindex;
} contexto_raw_socket;

// Cria um raw socket e retorna seu descritor
contexto_raw_socket criar_raw_socket(char* nome_interface_rede);

#endif