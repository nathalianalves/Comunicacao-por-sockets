#include "sockets.h"

// Cria um raw socket e retorna seu descritor de arquivo
int criaRawSocket(char *ifname) {
	int sock, ifindex;
	struct sockaddr_ll address = {0};
	
	// Cria um raw socket e guarda seu descritor em sock
	sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sock == -1){
		printf("ERRO: criação de socket falhou (verifique permissões)\n");
		exit(-1);
	}

	// ifindex := index da interface de rede solicitada
	ifindex = if_nametoindex(ifname);
	if (!ifindex) {
		printf("ERRO: index da interface de rede solicitada não foi encontrado\n");
		exit(-1);
	}

	address.sll_family = AF_PACKET;
	address.sll_protocol = htons(ETH_P_ALL);
	address.sll_ifindex = ifindex;
	if (bind(sock, (struct sockaddr*) &address, sizeof(address)) == -1) {
		printf("Erro ao fazer bind no socket\n");
		exit(-1);
	}

	struct packet_mreq mr = {0};
	mr.mr_ifindex = ifindex;
	mr.mr_type = PACKET_MR_PROMISC;
	// Não joga fora o que identifica como lixo: Modo promíscuo
	if (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
		printf("ERRO: verifique se a interface de rede foi especificada corretamente.\n");
		exit(-1);
	}

	return sock;
}

// Verifica se um protocolo eh valido
int protocoloValido(char* buffer, int tamanho_buffer) {
	if (tamanho_buffer <= 0) { return 0; }
	// insira a sua validação de protocolo aqui
	return buffer[0] == 0x7f;
}

// retorna -1 se deu timeout ou quantidade de bytes lidos
int recebeMensagem(int sock, int timeoutMillis, char* buffer, int tamanhoBuffer) {
	int bytesLidos;
	long long comeco = timestamp();
	struct timeval timeout = { .tv_sec = timeoutMillis/1000,
														 .tv_usec = (timeoutMillis%1000) * 1000 
														};

	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
	
	do {
			bytesLidos = recv(sock, buffer, tamanhoBuffer, 0);
			if (protocoloValido(buffer, bytesLidos)) { 
				return bytesLidos;
			}
	} while (timestamp() - comeco <= timeoutMillis);
	
	return -1;
}

int envia(int sock, const char* destino_mac_str, unsigned char *interface, const unsigned char* dados, int tamanho) {
	struct sockaddr_ll sa;
	struct ifreq ifr;
	unsigned char destino_mac[6];

	// converter string MAC para bytes
	sscanf(destino_mac_str, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
				 &destino_mac[0], &destino_mac[1], &destino_mac[2],
				 &destino_mac[3], &destino_mac[4], &destino_mac[5]);

	// nome da interface fixa (ou passe como argumento depois)
	strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);

	if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
			perror("ioctl");
			return -1;
	}

	memset(&sa, 0, sizeof(sa));
	sa.sll_ifindex = ifr.ifr_ifindex;
	sa.sll_halen = ETH_ALEN;
	memcpy(sa.sll_addr, destino_mac, 6);

	return sendto(sock, dados, tamanho, 0, (struct sockaddr*)&sa, sizeof(sa));
}

int recebe(int sock, unsigned char* dados, char* origem_mac_str) {
	struct sockaddr_ll sa;
	socklen_t sa_len = sizeof(sa);
	unsigned char buffer[2048];

	int lidos = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&sa, &sa_len);
	if (lidos < 0) {
			perror("recvfrom");
			return -1;
	}

	memcpy(dados, buffer, lidos);

	if (origem_mac_str) {
			sprintf(origem_mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",
							sa.sll_addr[0], sa.sll_addr[1], sa.sll_addr[2],
							sa.sll_addr[3], sa.sll_addr[4], sa.sll_addr[5]);
	}

	return lidos;
}