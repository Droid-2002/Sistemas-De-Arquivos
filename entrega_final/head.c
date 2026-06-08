/*
 * head.c - Implementação do TAD Cabeçalho.
 *
 * Gerencia leitura e escrita do registro de cabeçalho de 17 bytes
 * no início do arquivo binário, campo a campo (nunca como struct inteira,
 * para evitar problemas de padding/alinhamento).
 */

#include "head.h"
#include <stdio.h>

/* Inicializa cabeçalho com valores padrão:
 *  - status = '0' (inconsistente até confirmar escrita com sucesso)
 *  - topo = -1 (nenhum registro removido)
 *  - proxRRN = 0 (arquivo vazio)
 *  - contadores zerados
 */
void head_start(Head *h) {
    h->status          = STATUS_INCONSISTENTE;
    h->topo            = -1;
    h->proxRRN         = 0;
    h->nroEstacoes     = 0;
    h->nroParesEstacao = 0;
}

/*
 * Escreve o cabeçalho no início do arquivo, campo a campo na ordem exata:
 * status(1) | topo(4) | proxRRN(4) | nroEstacoes(4) | nroParesEstacao(4)
 */
HeadStatus head_escrever(FILE *fp, const Head *h) {
    if (fp == NULL || h == NULL) return HEAD_ERRO;

    /* posicionar no início do arquivo antes de gravar */
    fseek(fp, 0, SEEK_SET);

    if (fwrite(&h->status,          TAM_STATUS,            1, fp) != 1) return HEAD_ERRO;
    if (fwrite(&h->topo,            TAM_TOPO,              1, fp) != 1) return HEAD_ERRO;
    if (fwrite(&h->proxRRN,         TAM_PROX_RRN,          1, fp) != 1) return HEAD_ERRO;
    if (fwrite(&h->nroEstacoes,     TAM_NRO_ESTACOES,      1, fp) != 1) return HEAD_ERRO;
    if (fwrite(&h->nroParesEstacao, TAM_NRO_PARES_ESTACAO, 1, fp) != 1) return HEAD_ERRO;

    return HEAD_OK;
}

/*
 * Lê o cabeçalho do início do arquivo, campo a campo.
 * Retorna HEAD_EOF se o arquivo estiver vazio, HEAD_ERRO em caso de falha.
 */
HeadStatus head_ler(FILE *fp, Head *h) {
    if (fp == NULL || h == NULL) return HEAD_ERRO;

    fseek(fp, 0, SEEK_SET);

    /* leitura do status: se falhar aqui, verifica se é EOF real */
    if (fread(&h->status, TAM_STATUS, 1, fp) != 1) {
        if (feof(fp)) return HEAD_EOF;
        return HEAD_ERRO;
    }
    if (fread(&h->topo,            TAM_TOPO,              1, fp) != 1) return HEAD_ERRO;
    if (fread(&h->proxRRN,         TAM_PROX_RRN,          1, fp) != 1) return HEAD_ERRO;
    if (fread(&h->nroEstacoes,     TAM_NRO_ESTACOES,      1, fp) != 1) return HEAD_ERRO;
    if (fread(&h->nroParesEstacao, TAM_NRO_PARES_ESTACAO, 1, fp) != 1) return HEAD_ERRO;

    return HEAD_OK;
}

/* Define o valor de um campo char do cabeçalho (apenas 'status') */
HeadStatus head_set_char(Head *h, HeadCampo campo, char valor) {
    if (h == NULL) return HEAD_ERRO;
    if (campo == HEAD_CAMPO_STATUS) {
        h->status = valor;
        return HEAD_OK;
    }
    return HEAD_ERRO;
}

/* Define o valor de um campo int do cabeçalho */
HeadStatus head_set_int(Head *h, HeadCampo campo, int valor) {
    if (h == NULL) return HEAD_ERRO;
    switch (campo) {
        case HEAD_CAMPO_TOPO:
            h->topo = valor;
            return HEAD_OK;
        case HEAD_CAMPO_PROX_RRN:
            h->proxRRN = valor;
            return HEAD_OK;
        case HEAD_CAMPO_NRO_ESTACOES:
            h->nroEstacoes = valor;
            return HEAD_OK;
        case HEAD_CAMPO_NRO_PARES_ESTACAO:
            h->nroParesEstacao = valor;
            return HEAD_OK;
        default:
            return HEAD_ERRO;
    }
}
