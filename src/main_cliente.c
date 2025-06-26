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

    uint8_t tabuleiro[TAM_TABULEIRO*TAM_TABULEIRO];
    int encontrei_tesouro = 0;

    int lidos;
    Frame frame_envio, frame_recebimento, ultimo_frame_enviado;
    unsigned char buffer_recebimento[sizeof(Frame)];
    
    FILE *tesouro;
    uint32_t tamanho_tesouro;
    int num_tesouros_encontrados = 0;
    unsigned char nome_tesouro[TAM_MAX_DADOS];
    char *caminho_tesouros, caminho_completo_tesouro[TAM_MAX_DADOS];

    unsigned char movimento;
    int pedir_movimento = 1;
    int sequencia_esperada = 0;

    int jogo_acabou = 0;

    contexto_raw_socket ctx_socket = criar_raw_socket("veth0");
    if (ctx_socket.socket_fd < 0) {
        perror("Erro ao criar raw socket");
        return 1;
    }

    caminho_tesouros = argv[1];
    strncpy(caminho_completo_tesouro, caminho_tesouros, TAM_MAX_DADOS);
    
    while (!jogo_acabou) {
        lidos = receber_mensagem(ctx_socket, TIMEOUT_MILI, buffer_recebimento, sizeof(buffer_recebimento));
        if (lidos == -1) {
            enviar_frame(ctx_socket, ultimo_frame_enviado);
        } else if (lidos > 0) {
            if (desserializar_frame(&frame_recebimento, buffer_recebimento, lidos) == -1) {
                frame_envio = criar_frame(TIPO_NACK, 0, 0, 0);
                enviar_frame(ctx_socket, frame_envio);
                ultimo_frame_enviado = frame_envio;

                continue;
            };

            switch(frame_recebimento.tipo) {
                case TIPO_ACK:
                    switch (ultimo_frame_enviado.tipo) {
                        // DESENHAR TABULEIRO E PEDIR MOVIMENTO
                        case TIPO_DIREITA:
                        case TIPO_CIMA:
                        case TIPO_BAIXO:
                        case TIPO_ESQUERDA:
                            pedir_movimento = 1;
                            //frame_envio = criar_frame(TIPO_OK_ACK, 0, 0, 0);
                            //enviar_frame(ctx_socket, frame_envio);
                            //ultimo_frame_enviado = frame_envio;
                            break;                    
                    }
                    break;

                case TIPO_NACK:
                    enviar_frame(ctx_socket, ultimo_frame_enviado);
                    break;

                case TIPO_OK_ACK:
                    pedir_movimento = 1;

                    frame_envio = criar_frame(TIPO_ACK, 0, 0, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;
                    
                    break;
            
                case TIPO_TABULEIRO:

                    frame_envio = criar_frame(TIPO_ACK, 0, NULL, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;

                    memcpy(tabuleiro, frame_recebimento.dados, TAM_TABULEIRO*TAM_TABULEIRO);
                    imprimir_tabuleiro(tabuleiro);
                    if (!encontrei_tesouro) {
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

                        enviar_frame(ctx_socket, frame_envio);
                        ultimo_frame_enviado = frame_envio;
                        pedir_movimento = 0;
                    } else {
                        encontrei_tesouro = 0;
                    }
                    //memcpy(tabuleiro, frame_recebimento.dados, TAM_TABULEIRO*TAM_TABULEIRO);

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
                        }
    
                        frame_envio = criar_frame(TIPO_ACK, sequencia_esperada, NULL, 0);
                        enviar_frame(ctx_socket, frame_envio);
    
                        sequencia_esperada = 1 - sequencia_esperada;
    
                    } else {
                        printf("PACOTE FORA DE ORDEM! Recebido %d, esperado %d\n",frame_recebimento.sequencia, sequencia_esperada);
    
                        frame_envio = criar_frame(TIPO_ACK, 1 - sequencia_esperada, NULL, 0);
                        enviar_frame(ctx_socket, frame_envio);
                    }

                    break;
            
                case TIPO_TEXTO:
                case TIPO_IMAGEM:
                case TIPO_VIDEO:
                    strncpy(caminho_completo_tesouro, caminho_tesouros, TAM_MAX_DADOS);
                    extrair_nome_arquivo(frame_recebimento, nome_tesouro);
                    gerar_caminho_completo(caminho_completo_tesouro, caminho_tesouros, (char*)nome_tesouro);

                    tesouro = fopen(caminho_completo_tesouro, "wb");
                    if (!tesouro) {
                        printf("[main_cliente] Erro em gerar arquivo para tesouro (caminho_completo_tesouro: %s\n", caminho_completo_tesouro);
                        return -1;
                    }

                    sequencia_esperada = 0;

                    frame_envio = criar_frame(TIPO_ACK, 0, NULL, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;
                    
                    break;
            
                case TIPO_FIM_ARQUIVO:
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
                case TIPO_CIMA:
                case TIPO_BAIXO:        
                case TIPO_ESQUERDA:
                    frame_envio = criar_frame(TIPO_ACK, 0, 0, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;
    
                    break;
            
                case TIPO_ENCONTROU_TESOURO:
                    encontrei_tesouro = 1;

                    frame_envio = criar_frame(TIPO_ACK, 0, NULL, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;
                    
                    break;
            
                case TIPO_ERRO:
                    frame_envio = criar_frame(TIPO_ACK, 0, 0, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;

                    break;
            }
        }
    } 

    printf("Encontrei todos os tesouros. O jogo acabou!\n");
    close(ctx_socket.socket_fd);
    return 0;
}



