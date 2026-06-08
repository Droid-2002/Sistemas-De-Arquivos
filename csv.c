/*
 * csv.c - Implementação do TAD CSV.
 *
 * Parsing linha a linha do arquivo CSV, convertendo campos para a
 * struct Registro (inteiros ou strings conforme o campo).
 *
 * Campos vazios (duas vírgulas seguidas, ou campo antes do '\n') são
 * tratados como nulos: inteiro -1 ou string NULL.
 */

#include "csv.h"
#include "registro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* -----------------------------------------------------------------------
 * Helper: parsear linha CSV com suporte a campos vazios
 * ----------------------------------------------------------------------- */

/*
 * Divide uma linha CSV (modificada in-place) em até max_campos tokens,
 * substituindo cada vírgula por '\0' e guardando ponteiros em campos[].
 * Remove '\r' e '\n' do final.
 * Retorna a quantidade de campos encontrados.
 */
static int split_csv(char *linha, char **campos, int max_campos) {
    /* remover \r e \n do final */
    int len = (int)strlen(linha);
    while (len > 0 && (linha[len - 1] == '\n' || linha[len - 1] == '\r')) {
        linha[--len] = '\0';
    }

    int count = 0;
    campos[count++] = linha; /* primeiro campo começa no início */

    char *p = linha;
    while (*p != '\0' && count < max_campos) {
        if (*p == ',') {
            *p = '\0';       /* encerrar campo atual */
            campos[count++] = p + 1; /* próximo campo começa logo após */
        }
        p++;
    }
    return count;
}

/*
 * Converte string de campo para int.
 * Se o campo está vazio ("") ou é apenas espaço, retorna -1 (nulo).
 */
static int campo_para_int(const char *campo) {
    if (campo == NULL) return -1;
    /* pular espaços iniciais */
    while (isspace((unsigned char)*campo)) campo++;
    if (*campo == '\0') return -1;  /* campo vazio = nulo */
    return atoi(campo);
}

/*
 * Remove espaços iniciais e finais de uma string in-place.
 * Retorna ponteiro para o início da string sem espaços iniciais.
 */
static char *trim(char *s) {
    if (s == NULL) return s;
    /* espaços iniciais */
    while (isspace((unsigned char)*s)) s++;
    /* espaços finais */
    int len = (int)strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1])) {
        s[--len] = '\0';
    }
    return s;
}

/* -----------------------------------------------------------------------
 * Funções do TAD
 * ----------------------------------------------------------------------- */

CsvStatus csv_abrir(Csv *csv, const char *caminho) {
    csv->fp          = fopen(caminho, "r");
    csv->linha_atual = 0;

    if (csv->fp == NULL) return CSV_ERRO;
    return CSV_OK;
}

CsvStatus csv_pular_cabecalho(Csv *csv) {
    char linha[MAX_LINHA_CSV];
    if (fgets(linha, MAX_LINHA_CSV, csv->fp) == NULL) {
        if (feof(csv->fp)) return CSV_EOF;
        return CSV_ERRO;
    }
    csv->linha_atual++;
    return CSV_OK;
}

/*
 * Lê a próxima linha do CSV e popula todos os campos do Registro r.
 * Ordem esperada dos campos no CSV:
 *   [0] codEstacao
 *   [1] nomeEstacao
 *   [2] codLinha
 *   [3] nomeLinha
 *   [4] codProxEstacao
 *   [5] distProxEstacao
 *   [6] codLinhaIntegra
 *   [7] codEstIntegra
 */
CsvStatus csv_ler_registro(Csv *csv, Registro *r) {
    char linha[MAX_LINHA_CSV];

    if (fgets(linha, MAX_LINHA_CSV, csv->fp) == NULL) {
        if (feof(csv->fp)) return CSV_EOF;
        return CSV_ERRO;
    }
    csv->linha_atual++;

    /* dividir linha em campos */
    char *campos[8];
    int n = split_csv(linha, campos, 8);

    /* inicializar registro com valores padrão */
    *r = registro_start();

    /* [0] codEstacao - obrigatório, nunca nulo */
    if (n > 0) r->codEstacao = campo_para_int(campos[0]);

    /* [1] nomeEstacao - obrigatório */
    if (n > 1) {
        char *nome = trim(campos[1]);
        if (nome != NULL && nome[0] != '\0') {
            int tam = (int)strlen(nome);
            registro_set_nome_estacao(r, nome, tam);
        }
    }

    /* [2] codLinha */
    if (n > 2) r->codLinha = campo_para_int(campos[2]);

    /* [3] nomeLinha - pode ser nulo */
    if (n > 3) {
        char *nome = trim(campos[3]);
        if (nome != NULL && nome[0] != '\0') {
            int tam = (int)strlen(nome);
            registro_set_nome_linha(r, nome, tam);
        }
        /* se vazio, nomeLinha permanece NULL (nulo) */
    }

    /* [4] codProxEstacao */
    if (n > 4) r->codProxEstacao = campo_para_int(campos[4]);

    /* [5] distProxEstacao */
    if (n > 5) r->distProxEstacao = campo_para_int(campos[5]);

    /* [6] codLinhaIntegra */
    if (n > 6) r->codLinhaIntegra = campo_para_int(campos[6]);

    /* [7] codEstIntegra */
    if (n > 7) r->codEstIntegra = campo_para_int(campos[7]);

    return CSV_OK;
}

void csv_fechar(Csv *csv) {
    if (csv->fp != NULL) {
        fclose(csv->fp);
        csv->fp = NULL;
    }
}
