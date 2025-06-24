#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "protocolo.h"
#include "sockets.h"
#include "cliente.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Uso: sudo ./cliente <diretório em que os tesouros serão salvos>\n");
        return -1;
    }
    
    int enviados, lidos;
    contexto_raw_socket ctx_socket;
    Frame frame_envio, frame_recebimento, ultimo_frame_enviado;
    unsigned char movimento, dados[TAM_MAX_DADOS], buffer_recebimento[sizeof(Frame)];
    
    FILE *tesouro;
    int pedir_movimento = 1;
    int ultima_sequencia_recebida = -1;
    uint32_t tamanho_tesouro;
    unsigned char nome_tesouro[TAM_MAX_DADOS];
    char *caminho_tesouros, caminho_completo_tesouro[TAM_MAX_DADOS];
    int sequencia_esperada = 0;

    ctx_socket = criar_raw_socket("veth0");
    if (ctx_socket.socket_fd < 0) {
        perror("Erro ao criar raw socket");
        return 1;
    }

    caminho_tesouros = argv[1];
    strncpy(caminho_completo_tesouro, caminho_tesouros, TAM_MAX_DADOS);
    int num_tesouros_encontrados = 0;
    int jogo_acabou = 0;
    while (!jogo_acabou) {
        lidos = receber_mensagem(ctx_socket, TIMEOUT_MILI, buffer_recebimento, sizeof(buffer_recebimento));
        if (lidos == -1) {
            frame_envio = criar_frame(TIPO_NACK, 0, 0, 0);
            enviar_frame(ctx_socket, frame_envio);
            ultimo_frame_enviado = frame_envio;
        } 
        if ((lidos > 0) && (protocolo_eh_valido(buffer_recebimento, sizeof(buffer_recebimento)))) {
            if (desserializar_frame(&frame_recebimento, buffer_recebimento, lidos) == -1) {
                printf("Enviando NACK\n");

                frame_envio = criar_frame(TIPO_NACK, 0, 0, 0);
                enviar_frame(ctx_socket, frame_envio);
                ultimo_frame_enviado = frame_envio;

                continue;
            };

            switch(frame_recebimento.tipo) {
                case TIPO_ACK:
                    switch (ultimo_frame_enviado.tipo) {
                        case TIPO_DIREITA:
                        case TIPO_CIMA:
                        case TIPO_BAIXO:
                        case TIPO_ESQUERDA:
                            frame_envio = criar_frame(TIPO_OK_ACK, 0, 0, 0);
                            enviar_frame(ctx_socket, frame_envio);
                            ultimo_frame_enviado = frame_envio;
                            break;                    
                    }
                    printf("Recebi tipo ACK\n");
                    break;

                case TIPO_NACK:
                    printf("Recebi tipo NACK\n");
                    break;

                case TIPO_OK_ACK:
                    printf("Recebi tipo OK ACK\n");
                    pedir_movimento = 1;

                    break;
            
                case TIPO_TABULEIRO:
                    printf("Recebi tipo TABULEIRO\n");

                    imprimir_tabuleiro(frame_recebimento.dados);

                    if (pedir_movimento) {
                        movimento = ler_tecla_valida();
                        switch(movimento) {
                            case 'w':
                                frame_envio = criar_frame(TIPO_CIMA, 0, 0, 0);
                                break;

                            case 'a':
                                frame_envio = criar_frame(TIPO_ESQUERDA, 0, 0, 0);
                                break;

                            case 's':
                                frame_envio = criar_frame(TIPO_BAIXO, 0, 0, 0);
                                break;
                        
                            case 'd':
                                frame_envio = criar_frame(TIPO_DIREITA, 0, 0, 0);
                                break;
                        }
                    }

                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;

                    frame_envio = criar_frame(TIPO_ACK, 0, NULL, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;

                    break;

                case TIPO_TAMANHO:
                    processar_frame_tamanho(&frame_recebimento, &tamanho_tesouro);
                                    
                    if (obter_espaco_disponivel(caminho_tesouros) >= tamanho_tesouro) {
                       frame_envio = criar_frame(TIPO_OK_ACK, 0, 0, 0);
                    } else {
                       frame_envio = criar_frame(TIPO_ERRO, 0, 0, 0);
                    }

                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;

                    break;
            
                case TIPO_DADOS:
                    if (frame_recebimento.sequencia == sequencia_esperada) {
                        if (tesouro) {
                            fwrite(frame_recebimento.dados, 1, frame_recebimento.tamanho, tesouro);
                            printf("Dados recebidos seq=%d\n", sequencia_esperada);
                        }
    
                        frame_envio = criar_frame(TIPO_ACK, sequencia_esperada, NULL, 0);
                        enviar_frame(ctx_socket, frame_envio);
    
                        sequencia_esperada = 1 - sequencia_esperada;
    
                        printf("ACK enviado para seq=%d\n", frame_envio.sequencia);
                    } else {
                        printf("PACOTE FORA DE ORDEM! Recebido %d, esperado %d\n",frame_recebimento.sequencia, sequencia_esperada);
    
                        frame_envio = criar_frame(TIPO_ACK, 1 - sequencia_esperada, NULL, 0);
                        enviar_frame(ctx_socket, frame_envio);
                    }

                    break;
            
                case TIPO_TEXTO:
                    strncpy(caminho_completo_tesouro, caminho_tesouros, TAM_MAX_DADOS);
                    extrair_nome_arquivo(frame_recebimento, nome_tesouro);
                    gerar_caminho_completo(caminho_completo_tesouro, caminho_tesouros, (char*)nome_tesouro);

                    tesouro = fopen(caminho_completo_tesouro, "wb");
                    printf("caminho completo eh %s\n", caminho_completo_tesouro);
                    if (!tesouro) {
                        printf("[main_cliente] Erro em gerar arquivo para tesouro\n");
                        return -1;
                    }

                    sequencia_esperada = 0;

                    frame_envio = criar_frame(TIPO_ACK, 0, NULL, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;
                    
                    break;
            
                case TIPO_VIDEO:
                    printf("Recebi tipo VIDEO\n");
                    break;
            
                case TIPO_IMAGEM:
                    printf("Recebi tipo IMAGEM\n");
                    break;
            
                case TIPO_FIM_ARQUIVO:
                    printf("Recebi tipo FIM ARQUIVO. Enviando ACK para FIM_ARQUIVO\n");
                    pedir_movimento = 1;
                    frame_envio = criar_frame(TIPO_ACK, 0, NULL, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;

                    if (tesouro) {
                        fclose(tesouro);
                        tesouro = NULL;

                        pid_t pid = fork();
                        if (pid < 0) {
                            perror("fork para abrir arquivo");
                        } else if (pid == 0) {
                            execlp("xdg-open", "xdg-open", caminho_completo_tesouro, (char*)NULL);
                            perror("execlp xdg-open");
                            exit(1);
                        } 
                    }

                    num_tesouros_encontrados++;
                    if (num_tesouros_encontrados == NUM_TESOUROS) {
                        jogo_acabou = 1;
                    }
                    
                    break;
            
                case TIPO_DIREITA:
                    printf("Recebi tipo DIREITA\n");
                    break;
            
                case TIPO_CIMA:
                    printf("Recebi tipo CIMA\n");
                    break;
            
                case TIPO_BAIXO:
                    printf("Recebi tipo BAIXO\n");
                    break;
            
                case TIPO_ESQUERDA:
                    printf("Recebi tipo ESQUERDA\n");
                    break;
            
                case TIPO_LIVRE2:
                    printf("Recebi tipo LIVRE2\n");
                    pedir_movimento = 0;

                    frame_envio = criar_frame(TIPO_ACK, 0, NULL, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;
                    
                    break;
            
                case TIPO_ERRO:
                    printf("Recebi tipo ERRO\n");
                    break;
            }
        }
    } 

    printf("Encontrei todos os tesouros. O jogo acabou!\n");
    close(ctx_socket.socket_fd);
    return 0;
}



