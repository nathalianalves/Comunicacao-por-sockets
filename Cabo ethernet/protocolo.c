// protocolo.c
#include "protocolo.h"
#include <string.h>

// Cria um frame válido
Frame empacotar(uint8_t tipo, uint8_t sequencia, uint8_t* dados, uint8_t tamanho) {
    Frame f;
    f.marcador = MARCADOR_INICIO;
    f.tamanho = tamanho;
    f.sequencia = sequencia;
    f.tipo = tipo;
    if (tamanho > 0 && dados != NULL) {
        memcpy(f.dados, dados, tamanho);
    }
    f.checksum = calcular_checksum(&f);
    return f;
}

// Lê dados crus do buffer e preenche um frame. Retorna 0 para sucesso, -1 para erro.
int desempacotar(Frame* f, const uint8_t* buffer, int buflen) {
    // verifica tamanho minimo
    if (buflen < 5) {
	    return -1; 
    }

    // verifica marcador de inicio
    if (buffer[0] != MARCADOR_INICIO) {
        return -1;
    }

    // puxa os dados do buffer para o frame
    f->marcador = buffer[0];
    f->tamanho = buffer[1];
    f->sequencia = buffer[2];
    f->tipo = buffer[3];
    f->checksum = buffer[4];

    // verifica tamanho maximo de dados
    if ((f->tamanho > TAM_MAX_DADOS) || (buflen < 5 + f->tamanho)) {
        return -1;
    }

    // copia os dados
    memcpy(f->dados, &buffer[5], f->tamanho);

    // Verificar checksum
    uint8_t checksum = calcular_checksum(f);
    if (checksum != f->checksum) {
        return -1;
    }

    return 0;
}

// Calcula o checksum dos campos: tamanho, sequencia, tipo e dados
uint8_t calcular_checksum(const Frame* f) {
    uint16_t soma = 0;
    soma += f->tamanho;
    soma += f->sequencia;
    soma += f->tipo;
    for (int i = 0; i < f->tamanho; i++) {
        soma += f->dados[i];
    }
    return (uint8_t)(soma & 0xFF);  // Trunca para 8 bits
}