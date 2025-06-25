#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "protocolo.h"
#include "tabuleiro.h"
#include "servidor.h"

int main() {
    int tamanho_dados, lidos;
    unsigned char dados[TAM_MAX_DADOS], buffer_recebimento[sizeof(Frame)];
    Frame frame_envio, frame_recebimento, ultimo_frame_enviado;
    
    FILE* tesouro_atual = NULL;
    uint32_t tamanho_tesouro_atual;
    char caminho_tesouro_atual[TAM_MAX_DADOS], nome_tesouro_atual[64];
    int tesouro_sendo_enviado, encontrou_tesouro, num_tesouros_encontrados, esperando_permissao_tabuleiro, tipo_tesouro;

    int sequencia_envio = 0; 
    int sequencia_ack_esperada = 0; 
    int tentativas = 0;             
    const int MAX_TENTATIVAS = 5;
    
    int jogo_acabou = 0;

    contexto_raw_socket ctx_socket = criar_raw_socket("veth1");  
    if (ctx_socket.socket_fd < 0) {
        printf("[main_servidor] Erro ao criar raw socket.\n");
        return 1;
    }

    Jogo *jogo = criar_jogo();
    zerar_dados(dados, sizeof(dados));
    esconder_tesouros(jogo, dados);

    frame_envio = criar_frame(TIPO_TABULEIRO, 0, dados, TAM_TABULEIRO*TAM_TABULEIRO);
    enviar_frame(ctx_socket, frame_envio);
    ultimo_frame_enviado = frame_envio;

    imprimir_tabuleiro(jogo->tabuleiro);
    esperando_permissao_tabuleiro = 0;

    tesouro_sendo_enviado = 0;
    encontrou_tesouro = 0;
    num_tesouros_encontrados = 0;
    tipo_tesouro = -1;
    while (!jogo_acabou) {
        lidos = receber_mensagem(ctx_socket, TIMEOUT_MILI, buffer_recebimento, sizeof(buffer_recebimento));       
        if (lidos == -1) {
            enviar_frame(ctx_socket, ultimo_frame_enviado);
        } else if ((lidos > 0) && (protocolo_eh_valido(buffer_recebimento, sizeof(buffer_recebimento)))) {
            desserializar_frame(&frame_recebimento, buffer_recebimento, sizeof(buffer_recebimento));
            
            switch(frame_recebimento.tipo) {
                case TIPO_ACK:
                    if (esperando_permissao_tabuleiro) {
                    
                        zerar_dados(dados, sizeof(dados));
                        esconder_tesouros(jogo, dados);

                        frame_envio = criar_frame(TIPO_TABULEIRO, 0, dados, TAM_TABULEIRO*TAM_TABULEIRO);
                        enviar_frame(ctx_socket, frame_envio);
                        ultimo_frame_enviado = frame_envio;

                        esperando_permissao_tabuleiro = 0;

                    } else if (encontrou_tesouro) {   
                        
                        num_tesouros_encontrados++;
                        frame_envio = criar_frame_tamanho(num_tesouros_encontrados, caminho_tesouro_atual, &tamanho_tesouro_atual);

                        enviar_frame(ctx_socket, frame_envio);
                        ultimo_frame_enviado = frame_envio;
                        encontrou_tesouro = 0;
                    
                    } else if ((ultimo_frame_enviado.tipo == TIPO_TEXTO) || (ultimo_frame_enviado.tipo == TIPO_IMAGEM)  || (ultimo_frame_enviado.tipo == TIPO_VIDEO)) {

                        tesouro_atual = fopen(caminho_tesouro_atual, "rb");
                        
                        if (!tesouro_atual) {
                            printf("[main_servidor] Erro ao abrir o tesouro\n");
                        } else {
                            sequencia_envio = 0;
                            sequencia_ack_esperada = 0;
                            tentativas = 0;
                            tesouro_sendo_enviado = 1;
    
                            zerar_dados(dados, sizeof(dados));
                            tamanho_dados = fread(dados, 1, TAM_MAX_DADOS, tesouro_atual);
    
                            frame_envio = criar_frame(TIPO_DADOS, 0, dados, tamanho_dados);
                            enviar_frame(ctx_socket, frame_envio);
                            ultimo_frame_enviado = frame_envio;
                        } 

                    } else if (tesouro_sendo_enviado) {
                        if (frame_recebimento.sequencia == sequencia_ack_esperada) {
                            tentativas = 0;  
        
                            if (feof(tesouro_atual)) {

                                frame_envio = criar_frame(TIPO_FIM_ARQUIVO, sequencia_envio, 0, 0);
                                enviar_frame(ctx_socket, frame_envio);
                                ultimo_frame_enviado = frame_envio;
            
                                tesouro_sendo_enviado = 0;
                                esperando_permissao_tabuleiro = 1;
                                fclose(tesouro_atual);
                                tesouro_atual = NULL;

                                if (num_tesouros_encontrados == NUM_TESOUROS) {
                                    jogo_acabou = 1;
                                }

                            } else {

                                sequencia_envio = 1 - sequencia_envio;
                                sequencia_ack_esperada = sequencia_envio;
            
                                zerar_dados(dados, sizeof(dados));
                                tamanho_dados = fread(dados, 1, TAM_MAX_DADOS, tesouro_atual);
            
                                frame_envio = criar_frame(TIPO_DADOS, sequencia_envio, dados, tamanho_dados);
                                enviar_frame(ctx_socket, frame_envio);
                                ultimo_frame_enviado = frame_envio;

                            }
                        } else {
                            printf("[main_servidor] Ack incorreto: recebido %d, esperado %d\n", frame_recebimento.sequencia, sequencia_ack_esperada);
                            if (++tentativas >= MAX_TENTATIVAS) {
                                printf("[main_servidor] FALHA: Muitas retransmissões\n");
                                tesouro_sendo_enviado = 0;
                                fclose(tesouro_atual);
                            } else {
                                printf("[main_servidor] Reenviando pacote seq=%d (tentativa %d/%d)\n", sequencia_envio, tentativas, MAX_TENTATIVAS);
                                enviar_frame(ctx_socket, ultimo_frame_enviado);
                            }
                        }
                    } else if (ultimo_frame_enviado.tipo == TIPO_FIM_ARQUIVO) {
                        zerar_dados(dados, sizeof(dados));
                        esconder_tesouros(jogo, dados);

                        frame_envio = criar_frame(TIPO_TABULEIRO, 0, dados, TAM_TABULEIRO*TAM_TABULEIRO);
                        enviar_frame(ctx_socket, frame_envio);
                        ultimo_frame_enviado = frame_envio;

                    }

                    break;

                case TIPO_NACK:
                    enviar_frame(ctx_socket, ultimo_frame_enviado);                    
                    break;

                case TIPO_OK_ACK:
                    if (ultimo_frame_enviado.tipo == TIPO_TAMANHO) {
                        zerar_dados(dados, sizeof(dados));
                        obter_nome_tesouro(nome_tesouro_atual, caminho_tesouro_atual);
                        memcpy(dados, nome_tesouro_atual, sizeof(nome_tesouro_atual));

                        tipo_tesouro = extrair_tipo_tesouro(caminho_tesouro_atual);
                        switch (tipo_tesouro) {
                            case -1:
                                printf("[main_servidor] Erro em extrair_tipo_tesouro\n");
                                break;

                            case 0:
                                frame_envio = criar_frame(TIPO_TEXTO, 0, dados, strlen(caminho_tesouro_atual)+1);
                                break;
                            
                            case 1:
                                frame_envio = criar_frame(TIPO_IMAGEM, 0, dados, strlen(caminho_tesouro_atual)+1);
                                break;

                            case 2:
                                frame_envio = criar_frame(TIPO_VIDEO, 0, dados, strlen(caminho_tesouro_atual)+1);
                                break;
                        }
                        
                        enviar_frame(ctx_socket, frame_envio);
                        ultimo_frame_enviado = frame_envio;
                    
                    } 
                    break;
            
                case TIPO_TABULEIRO:
                case TIPO_TAMANHO:
                case TIPO_DADOS:
                case TIPO_TEXTO:
                case TIPO_VIDEO:
                case TIPO_IMAGEM:
                case TIPO_FIM_ARQUIVO:
                    frame_envio = criar_frame(TIPO_ACK, 0, 0, 0);
                    enviar_frame(ctx_socket, frame_envio);
                    ultimo_frame_enviado = frame_envio;

                    break;
            
                case TIPO_DIREITA:
                    if (verificar_movimentacao(jogo, 0)) {
                        efetuar_movimentacao(jogo, 0, &encontrou_tesouro);
                        
                        if (encontrou_tesouro) {
                            frame_envio = criar_frame(TIPO_ENCONTROU_TESOURO, 0, 0, 0);
                        } else {
                            frame_envio = criar_frame(TIPO_OK_ACK, 0, 0, 0);
                        }   
                    } else {
                        frame_envio = criar_frame(TIPO_ACK, 0, 0, 0);
                    }

                    enviar_frame(ctx_socket,frame_envio);
                    ultimo_frame_enviado = frame_envio;
                    esperando_permissao_tabuleiro = 1;

                    break;
            
                case TIPO_CIMA:
                    if (verificar_movimentacao(jogo, 1)) {
                        efetuar_movimentacao(jogo, 1, &encontrou_tesouro);
                        
                        if (encontrou_tesouro) {
                            frame_envio = criar_frame(TIPO_ENCONTROU_TESOURO, 0, 0, 0);
                        } else {
                            frame_envio = criar_frame(TIPO_OK_ACK, 0, 0, 0);
                        }   
                    } else {
                        frame_envio = criar_frame(TIPO_ACK, 0, 0, 0);
                    }
                    
                    enviar_frame(ctx_socket,frame_envio);
                    ultimo_frame_enviado = frame_envio;
                    esperando_permissao_tabuleiro = 1;
                                        
                    break;
            
                case TIPO_BAIXO:
                    if (verificar_movimentacao(jogo, 2)) {
                        efetuar_movimentacao(jogo, 2, &encontrou_tesouro);
                        
                        if (encontrou_tesouro) {
                            frame_envio = criar_frame(TIPO_ENCONTROU_TESOURO, 0, 0, 0);
                        } else {
                            frame_envio = criar_frame(TIPO_OK_ACK, 0, 0, 0);
                        }
                    } else {
                        frame_envio = criar_frame(TIPO_ACK, 0, 0, 0);

                    }

                    enviar_frame(ctx_socket,frame_envio);
                    ultimo_frame_enviado = frame_envio;
                    esperando_permissao_tabuleiro = 1;

                    break;
            
                case TIPO_ESQUERDA:
                    if (verificar_movimentacao(jogo, 3)) {
                        efetuar_movimentacao(jogo, 3, &encontrou_tesouro);

                        if (encontrou_tesouro) {
                            frame_envio = criar_frame(TIPO_ENCONTROU_TESOURO, 0, 0, 0);
                        } else {
                            frame_envio = criar_frame(TIPO_OK_ACK, 0, 0, 0);
                        }
                    } else {
                        frame_envio = criar_frame(TIPO_ACK, 0, 0, 0);
                    }
                    
                    esperando_permissao_tabuleiro = 1;
                    enviar_frame(ctx_socket,frame_envio);
                    ultimo_frame_enviado = frame_envio;

                    break;
            
                case TIPO_ENCONTROU_TESOURO:
                    break;
            
                case TIPO_ERRO:
                    printf("[main_servidor] Erro no cliente.\n");
                    break;
            }     
        }

        imprimir_tabuleiro(jogo->tabuleiro);
    }

    printf("Usuário encontrou todos os tesouros. O jogo acabou!\n");
    close(ctx_socket.socket_fd);
    return 0;
}
