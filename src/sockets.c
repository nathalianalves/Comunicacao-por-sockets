#include "sockets.h"

// Cria raw socket e retorna seu descritor
contexto_raw_socket criar_raw_socket(char* nome_interface_rede) {
	contexto_raw_socket novo_raw_socket;
	
    novo_raw_socket.socket_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (novo_raw_socket.socket_fd == -1) {
        printf("[cria_raw_socket] Erro ao criar socket. Verifique permissões.\n");
        exit(-1);
    }
 
    novo_raw_socket.ifindex = if_nametoindex(nome_interface_rede);
 
    struct sockaddr_ll endereco = {0};
    endereco.sll_family = AF_PACKET;
    endereco.sll_protocol = htons(ETH_P_ALL);
    endereco.sll_ifindex = novo_raw_socket.ifindex;
    if (bind(novo_raw_socket.socket_fd, (struct sockaddr*) &endereco, sizeof(endereco)) == -1) {
        printf("[cria_raw_socket] Erro ao fazer bind no socket\n");
        exit(-1);
    }
 
    struct packet_mreq mr = {0};
    mr.mr_ifindex = novo_raw_socket.ifindex;
    mr.mr_type = PACKET_MR_PROMISC;
    if (setsockopt(novo_raw_socket.socket_fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
        printf("[cria_raw_socket] Erro ao fazer setsockopt. Verifique se a interface de rede foi especificada corretamente.\n");
        exit(-1);
    }
 
    return novo_raw_socket;
}