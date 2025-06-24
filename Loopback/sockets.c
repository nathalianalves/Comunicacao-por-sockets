#include "sockets.h"

// Cria raw socket e retorna seu descritor
contexto_raw_socket criar_raw_socket(char* nome_interface_rede) {
	contexto_raw_socket novo_raw_socket;
	
	// Cria arquivo para o socket sem qualquer protocolo
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
    // Inicializa socket
    if (bind(novo_raw_socket.socket_fd, (struct sockaddr*) &endereco, sizeof(endereco)) == -1) {
        printf("[cria_raw_socket] Erro ao fazer bind no socket\n");
        exit(-1);
    }
 
    struct packet_mreq mr = {0};
    mr.mr_ifindex = novo_raw_socket.ifindex;
    mr.mr_type = PACKET_MR_PROMISC;
    // Não joga fora o que identifica como lixo: Modo promíscuo
    if (setsockopt(novo_raw_socket.socket_fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) == -1) {
        printf("[cria_raw_socket] Erro ao fazer setsockopt. Verifique se a interface de rede foi especificada corretamente.\n");
        exit(-1);
    }
 
    return novo_raw_socket;
}

// Envia frame dados no socket passado como parâmetro para a maquina com MAC dest_mac
// Retorno: para sucesso, a quantidade de bytes enviados. Para erro, -1.
/*int enviar_frame(contexto_raw_socket ctx_socket, const char* str_mac_dest, const unsigned char* dados, int tamanho) {
    struct sockaddr_ll sa = {0};
    unsigned char mac_dest[6];

    // mac := dest_mac em string
    if (sscanf(str_mac_dest, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac_dest[0], &mac_dest[1], &mac_dest[2], &mac_dest[3], &mac_dest[4], &mac_dest[5]) != 6) {
        printf("[enviar_frame] Erro: formato de MAC inválido.\n");
        return -1;
    }

    // Configuração da estrutura sockaddr_ll
    sa.sll_family = AF_PACKET;
    sa.sll_ifindex = ctx_socket.ifindex;
    sa.sll_halen = ETH_ALEN;
    memcpy(sa.sll_addr, mac_dest, ETH_ALEN);

    return sendto(ctx_socket.socket_fd, dados, tamanho, 0, (struct sockaddr*)&sa, sizeof(sa));
} */

// Retorno: para sucesso, a quantidade de bytes lidos. Para erro, -1.
/*int receber_mensagem(contexto_raw_socket ctx_socket, int timeout_milissegundos, unsigned char *buffer, int tamanho_buffer) {
    long long comeco = timestamp();
    struct timeval timeout = {.tv_sec = timeout_milissegundos/1000,
                              .tv_usec = (timeout_milissegundos % 1000) * 1000};

    setsockopt(ctx_socket.socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

    int bytes_lidos;
    do {
        bytes_lidos = recv(ctx_socket.socket_fd, buffer, tamanho_buffer, 0);
        printf("[receber_mensagem] recebi algo (%s).\n", buffer);
        return bytes_lidos;
    } while (timestamp() - comeco <= timeout_milissegundos);

    return -1; // Se execução chegou aqui, é porque deu erro
}*/