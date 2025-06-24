#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "protocolo.h"
#include "tabuleiro.h"
#include "servidor.h"

int main() {
    int tamanho_dados, enviados, lidos, tabuleiro_foi_atualizado, esperando_permissao_tabuleiro;
    unsigned char dados[TAM_MAX_DADOS], buffer_recebimento[sizeof(Frame)];
    contexto_raw_socket ctx_socket;
    Frame frame_envio, frame_recebimento, ultimo_frame_enviado;
    Jogo *jogo;

    FILE* tesouro_atual = NULL;
    uint32_t tamanho_tesouro_atual, num_pacotes_tesouro, ultima_sequencia_enviada;
    char nome_tesouro_atual[TAM_MAX_DADOS];
    int tesouro_sendo_enviado, encontrou_tesouro, num_tesouros_encontrados;
    int sequencia_esperada = 0;
    int sequencia_envio = 0; 
    int sequencia_ack_esperada = 0; 
    int tentativas = 0;             
    const int MAX_TENTATIVAS = 5;
    int jogo_acabou = 0;

    ctx_socket = criar_raw_socket("veth1");  
    if (ctx_socket.socket_fd < 0) {
        printf("[main_servidor] Erro ao criar raw socket.\n");
        return 1;
    }

    jogo = criar_jogo();
    zerar_dados(dados, sizeof(dados));
    esconder_tesouros(jogo, dados);

    frame_envio = criar_frame(TIPO_TABULEIRO, 0, dados, TAM_TABULEIRO*TAM_TABULEIRO);
    enviar_frame(ctx_socket, frame_envio);
    ultimo_frame_enviado = frame_envio;

    imprimir_tabuleiro(jogo->tabuleiro);
    tabuleiro_foi_atualizado = 0; 
    esperando_permissao_tabuleiro = 0;

    num_pacotes_tesouro = 0;
    ultima_sequencia_enviada = 0;
    tesouro_sendo_enviado = 0;
    encontrou_tesouro = 0;
    num_tesouros_encontrados = 0;
    while (!jogo_acabou) {
        lidos = receber_mensagem(ctx_socket, TIMEOUT_MILI, buffer_recebimento, sizeof(buffer_recebimento));       
        if (lidos == -1) {
            frame_envio = criar_frame(TIPO_NACK, 0, 0, 0);
            enviar_frame(ctx_socket, frame_envio);
            ultimo_frame_enviado = frame_envio;
        }
        
        if ((lidos > 0) && (protocolo_eh_valido(buffer_recebimento, sizeof(buffer_recebimento)))){
            desserializar_frame(&frame_recebimento, buffer_recebimento, sizeof(buffer_recebimento));
            
            switch(frame_recebimento.tipo) {
                case TIPO_ACK:
                    printf("Recebi tipo ACK e o ultimo tipo que enviei foi %d\n", ultimo_frame_enviado.tipo);
        
                    if (esperando_permissao_tabuleiro) {
                    
                        zerar_dados(dados, sizeof(dados));
                        esconder_tesouros(jogo, dados);

                        frame_envio = criar_frame(TIPO_TABULEIRO, 0, dados, TAM_TABULEIRO*TAM_TABULEIRO);
                        enviar_frame(ctx_socket, frame_envio);
                        ultimo_frame_enviado = frame_envio;

                        esperando_permissao_tabuleiro = 0;

                    } else if (encontrou_tesouro) {   
                        
                        num_tesouros_encontrados++;
                        frame_envio = criar_frame_tamanho(num_tesouros_encontrados, nome_tesouro_atual, &tamanho_tesouro_atual);

                        enviar_frame(ctx_socket, frame_envio);
                        ultimo_frame_enviado = frame_envio;
                        encontrou_tesouro = 0;
                    
                    } else if (ultimo_frame_enviado.tipo == TIPO_TEXTO) {

                        tesouro_atual = fopen(nome_tesouro_atual, "rb");
                        
                        if (!tesouro_atual) {
                            printf("[main_servidor] Erro ao abrir o tesouro\n");
                        } else {
                            sequencia_envio = 0;
                            sequencia_ack_esperada = 0;
                            tentativas = 0;
                            tesouro_sendo_enviado = 1;
    
                            zerar_dados(dados, sizeof(dados));
                            tamanho_dados = fread(dados, 1, TAM_MAX_DADOS, tesouro_atual);
    
                            printf("Iniciando transferência seq=0\n");
                            frame_envio = criar_frame(TIPO_DADOS, 0, dados, tamanho_dados);
                            enviar_frame(ctx_socket, frame_envio);
                            ultimo_frame_enviado = frame_envio;
                        } 

                    } else if (tesouro_sendo_enviado) {
                        if (frame_recebimento.sequencia == sequencia_ack_esperada) {
                            printf("ACK CORRETO seq=%d\n", sequencia_ack_esperada);
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
                                    printf("Jogo acabou!\n");
                                    jogo_acabou = 1;
                                }

                            } else {

                                sequencia_envio = 1 - sequencia_envio;
                                sequencia_ack_esperada = sequencia_envio;
            
                                zerar_dados(dados, sizeof(dados));
                                tamanho_dados = fread(dados, 1, TAM_MAX_DADOS, tesouro_atual);
            
                                printf("Enviando novo pacote seq=%d\n", sequencia_envio);
                                frame_envio = criar_frame(TIPO_DADOS, sequencia_envio, dados, tamanho_dados);
                                enviar_frame(ctx_socket, frame_envio);
                                ultimo_frame_enviado = frame_envio;

                            }
                        } else {
                            printf("ACK INCORRETO: recebido %d, esperado %d\n", frame_recebimento.sequencia, sequencia_ack_esperada);
                            if (++tentativas >= MAX_TENTATIVAS) {
                                printf("FALHA: Muitas retransmissões\n");
                                tesouro_sendo_enviado = 0;
                                fclose(tesouro_atual);
                            } else {
                                printf("Reenviando pacote seq=%d (tentativa %d/%d)\n", sequencia_envio, tentativas, MAX_TENTATIVAS);
                                enviar_frame(ctx_socket, ultimo_frame_enviado);
                            }
                        }
                    } else if (ultimo_frame_enviado.tipo == TIPO_FIM_ARQUIVO) {
                        zerar_dados(dados, sizeof(dados));
                        esconder_tesouros(jogo, dados);

                        frame_envio = criar_frame(TIPO_TABULEIRO, 0, dados, TAM_TABULEIRO*TAM_TABULEIRO);
                        enviar_frame(ctx_socket, frame_envio);
                        ultimo_frame_enviado = frame_envio;

                        tabuleiro_foi_atualizado = 1;
                    }

                    break;

                case TIPO_NACK:
                    printf("Recebi tipo NACK\n");
                    enviar_frame(ctx_socket, ultimo_frame_enviado);                    
                    break;

                case TIPO_OK_ACK:
                    printf("Recebi tipo OK ACK\n");
                    if (ultimo_frame_enviado.tipo == TIPO_TAMANHO) {
                        zerar_dados(dados, sizeof(dados));
                        memcpy(dados, nome_tesouro_atual, sizeof(nome_tesouro_atual));

                        frame_envio = criar_frame(TIPO_TEXTO, 0, dados, strlen(nome_tesouro_atual)+1);
                        enviar_frame(ctx_socket, frame_envio);
                        ultimo_frame_enviado = frame_envio;
                    
                    } 
                    break;
            
                case TIPO_TABULEIRO:
                    printf("Recebi tipo TABULEIRO\n");
                    break;

                case TIPO_TAMANHO:
                    printf("Recebi tipo TAMANHO\n");
                    break;
            
                case TIPO_DADOS:
                    printf("Recebi tipo DADOS\n");
                    break;
            
                case TIPO_TEXTO:
                    printf("Recebi tipo TEXTO\n");
                    break;
            
                case TIPO_VIDEO:
                    printf("Recebi tipo VIDEO\n");
                    break;
            
                case TIPO_IMAGEM:
                    printf("Recebi tipo IMAGEM\n");
                    break;
            
                case TIPO_FIM_ARQUIVO:
                    printf("Recebi tipo FIM ARQUIVO\n");
                    break;
            
                case TIPO_DIREITA:
                    printf("Recebi tipo DIREITA\n");
                    if (verificar_movimentacao(jogo, 0)) {
                        efetuar_movimentacao(jogo, 0, &encontrou_tesouro);
                        tabuleiro_foi_atualizado = 1;          
                        
                        if (encontrou_tesouro) {
                            frame_envio = criar_frame(TIPO_LIVRE2, 0, 0, 0);
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
                    printf("Recebi tipo CIMA\n");
                    if (verificar_movimentacao(jogo, 1)) {
                        efetuar_movimentacao(jogo, 1, &encontrou_tesouro);
                        tabuleiro_foi_atualizado = 1;
                        
                        if (encontrou_tesouro) {
                            frame_envio = criar_frame(TIPO_LIVRE2, 0, 0, 0);
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
                    printf("Recebi tipo BAIXO\n");
                    if (verificar_movimentacao(jogo, 2)) {
                        efetuar_movimentacao(jogo, 2, &encontrou_tesouro);
                        tabuleiro_foi_atualizado = 1;
                        
                        if (encontrou_tesouro) {
                            frame_envio = criar_frame(TIPO_LIVRE2, 0, 0, 0);
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
                    printf("Recebi tipo ESQUERDA\n");
                    if (verificar_movimentacao(jogo, 3)) {
                        efetuar_movimentacao(jogo, 3, &encontrou_tesouro);
                        tabuleiro_foi_atualizado = 1;

                        if (encontrou_tesouro) {
                            frame_envio = criar_frame(TIPO_LIVRE2, 0, 0, 0);
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
            
                case TIPO_LIVRE2:
                    printf("Recebi tipo LIVRE2 e o tipo do meu ultimo frame eh %d\n", ultimo_frame_enviado.tipo);
                    break;
            
                case TIPO_ERRO:
                    printf("Recebi tipo ERRO\n");
                    break;
            }     
        }

        imprimir_tabuleiro(jogo->tabuleiro);
    }

    printf("Usuário encontrou todos os tesouros. O jogo acabou!\n");
    close(ctx_socket.socket_fd);
    return 0;
}
