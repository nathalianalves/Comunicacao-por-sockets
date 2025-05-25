#ifndef _PROTOCOLO_
#define _PROTOCOLO_

#define MARCADOR_INICIO 0x07E

typedef struct {
	unsigned char sinc;
	unsigned char tamanho : 7;
	unsigned char sequencia : 5;
	unsigned char tipo : 4;
	unsigned char checksum;
	unsigned char dados[];
} __attribute__((packed)) mensagem;

#endif