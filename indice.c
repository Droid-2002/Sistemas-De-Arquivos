/*
 * indice.c - Implementação do TAD Índice Primário.
 *
 * Todas as entradas são mantidas ordenadas de forma crescente por
 * codEstacao, o que permite busca binária em RAM e gravação direta
 * já ordenada em disco.
 */

#include "indice.h"
#include <stdio.h>
#include <stdlib.h>

#define INDICE_CAP_INICIAL 32

/* -----------------------------------------------------------------------
 * Inicialização e liberação
 * ----------------------------------------------------------------------- */

void indice_start(Indice *idx) {
    if (idx == NULL) return;
    idx->itens  = NULL;
    idx->n      = 0;
    idx->cap    = 0;
    idx->status = INDICE_STATUS_INCONSISTENTE;
}

void indice_free(Indice *idx) {
    if (idx == NULL) return;
    free(idx->itens);
    idx->itens = NULL;
    idx->n     = 0;
    idx->cap   = 0;
}

/* Garante capacidade para ao menos (idx->n + 1) entradas. */
static IndiceStatus indice_garantir_capacidade(Indice *idx) {
    if (idx->n < idx->cap) return INDICE_OK;

    int nova_cap = (idx->cap == 0) ? INDICE_CAP_INICIAL : idx->cap * 2;
    IndiceEntrada *novo = (IndiceEntrada *)realloc(idx->itens,
                                                   nova_cap * sizeof(IndiceEntrada));
    if (novo == NULL) return INDICE_ERRO;

    idx->itens = novo;
    idx->cap   = nova_cap;
    return INDICE_OK;
}

/* -----------------------------------------------------------------------
 * Carga e gravação
 * ----------------------------------------------------------------------- */

IndiceStatus indice_carregar(Indice *idx, const char *caminho) {
    if (idx == NULL || caminho == NULL) return INDICE_ERRO;

    indice_start(idx);

    FILE *fp = fopen(caminho, "rb");
    if (fp == NULL) return INDICE_ERRO;

    /* ler status (1 byte) */
    if (fread(&idx->status, TAM_INDICE_STATUS, 1, fp) != 1) {
        fclose(fp);
        return INDICE_ERRO;
    }
    if (idx->status == INDICE_STATUS_INCONSISTENTE) {
        fclose(fp);
        return INDICE_ERRO;
    }

    /* ler entradas até o fim do arquivo */
    int cod, rrn;
    while (fread(&cod, TAM_INDICE_COD, 1, fp) == 1) {
        if (fread(&rrn, TAM_INDICE_RRN, 1, fp) != 1) {
            fclose(fp);
            indice_free(idx);
            return INDICE_ERRO;
        }
        if (indice_garantir_capacidade(idx) != INDICE_OK) {
            fclose(fp);
            indice_free(idx);
            return INDICE_ERRO;
        }
        idx->itens[idx->n].codEstacao = cod;
        idx->itens[idx->n].rrn        = rrn;
        idx->n++;
    }

    fclose(fp);
    return INDICE_OK;
}

IndiceStatus indice_salvar(const Indice *idx, const char *caminho) {
    if (idx == NULL || caminho == NULL) return INDICE_ERRO;

    FILE *fp = fopen(caminho, "wb");
    if (fp == NULL) return INDICE_ERRO;

    char status = INDICE_STATUS_CONSISTENTE;
    if (fwrite(&status, TAM_INDICE_STATUS, 1, fp) != 1) {
        fclose(fp);
        return INDICE_ERRO;
    }

    for (int i = 0; i < idx->n; i++) {
        if (fwrite(&idx->itens[i].codEstacao, TAM_INDICE_COD, 1, fp) != 1 ||
            fwrite(&idx->itens[i].rrn,        TAM_INDICE_RRN, 1, fp) != 1) {
            fclose(fp);
            return INDICE_ERRO;
        }
    }

    fclose(fp);
    return INDICE_OK;
}

/* -----------------------------------------------------------------------
 * Busca, inserção e remoção
 * ----------------------------------------------------------------------- */

/*
 * Busca binária interna: retorna em *pos o índice onde a chave está
 * (se encontrada) ou onde ela deveria ser inserida (se não encontrada).
 * Retorna 1 se encontrou, 0 caso contrário.
 */
static int indice_busca_pos(const Indice *idx, int codEstacao, int *pos) {
    int ini = 0, fim = idx->n - 1;
    while (ini <= fim) {
        int meio = ini + (fim - ini) / 2;
        int chave = idx->itens[meio].codEstacao;
        if (chave == codEstacao) {
            *pos = meio;
            return 1;
        } else if (chave < codEstacao) {
            ini = meio + 1;
        } else {
            fim = meio - 1;
        }
    }
    *pos = ini; /* ponto de inserção */
    return 0;
}

int indice_buscar(const Indice *idx, int codEstacao) {
    if (idx == NULL) return -1;
    int pos;
    if (indice_busca_pos(idx, codEstacao, &pos)) {
        return idx->itens[pos].rrn;
    }
    return -1;
}

IndiceStatus indice_inserir(Indice *idx, int codEstacao, int rrn) {
    if (idx == NULL) return INDICE_ERRO;

    int pos;
    indice_busca_pos(idx, codEstacao, &pos); /* posição de inserção ordenada */

    if (indice_garantir_capacidade(idx) != INDICE_OK) return INDICE_ERRO;

    /* abre espaço deslocando as entradas seguintes uma posição à direita */
    for (int i = idx->n; i > pos; i--) {
        idx->itens[i] = idx->itens[i - 1];
    }
    idx->itens[pos].codEstacao = codEstacao;
    idx->itens[pos].rrn        = rrn;
    idx->n++;
    return INDICE_OK;
}

IndiceStatus indice_remover(Indice *idx, int codEstacao) {
    if (idx == NULL) return INDICE_ERRO;

    int pos;
    if (!indice_busca_pos(idx, codEstacao, &pos)) {
        return INDICE_EOF; /* não existia */
    }

    /* desloca as entradas seguintes uma posição à esquerda */
    for (int i = pos; i < idx->n - 1; i++) {
        idx->itens[i] = idx->itens[i + 1];
    }
    idx->n--;
    return INDICE_OK;
}
