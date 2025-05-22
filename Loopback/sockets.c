#include "sockets.h"

// Cria um raw socket e retorna seu descritor
int criaRawSocket(char* nomeInterface) {
	// Cria arquivo para o socket sem qualquer protocolo
	int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock == -1) {
			printf("ERRO: criaRawSocket falhou (verifique permissões)\n");
			exit(-1);
	}

	int ifindex = if_nametoindex(nomeInterface);

	struct sockaddr_ll endereco = {0};
	endereco.sll_family = AF_PACKET;
	endereco.sll_protocol = htons(ETH_P_ALL);
	endereco.sll_ifindex = ifindex;
	// Inicializa socket
	if (bind(sock, (struct sockaddr*) &endereco, sizeof(endereco)) == -1) {
			printf("ERRO: bind em criaRawSocket falhou\n");
			exit(-1);
	}

	struct packet_mreq mr = {0};
	mr.mr_ifindex = ifindex;
	mr.mr_type = PACKET_MR_PROMISC;
	// Não joga fora o que identifica como lixo: Modo promíscuo
	if (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
			printf("ERRO: setsockopt em criaRawSocket falhou (verifique se a interface de rede foi especificada corretamente)\n");
			exit(-1);
	}

	return sock;
}