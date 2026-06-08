/*
 * csv.h - TAD CSV: funções para abertura e parsing do arquivo CSV de entrada.
 *
 * O CSV possui uma linha de cabeçalho (descartada) seguida de linhas com
 * campos separados por vírgula na ordem:
 *   codEstacao, nomeEstacao, codLinha, nomeLinha, codProxEstacao,
 *   distProxEstacao, codLinhaIntegra, codEstIntegra
 *
 * Campos vazios indicam valores nulos.
 */

#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include "registro.h"

/* Tamanho máximo de uma linha do CSV */
#define MAX_LINHA_CSV 1024

/* Códigos de retorno */
typedef enum {
    CSV_OK   =  1,
    CSV_EOF  =  0,
    CSV_ERRO = -1
} CsvStatus;

/* Struct de controle do arquivo CSV */
typedef struct {
    FILE *fp;
    int   linha_atual; /* contador de linhas lidas (para mensagens de erro) */
} Csv;

/* Abre o CSV para leitura. Retorna CSV_ERRO se falhar. */
CsvStatus csv_abrir(Csv *csv, const char *caminho);

/* Descarta a primeira linha (cabeçalho de colunas). */
CsvStatus csv_pular_cabecalho(Csv *csv);

/*
 * Lê a próxima linha do CSV e preenche o Registro r.
 * Retorna CSV_EOF ao atingir o fim do arquivo.
 * Trata campos vazios como nulos conforme a especificação.
 */
CsvStatus csv_ler_registro(Csv *csv, Registro *r);

/* Fecha o arquivo CSV. */
void csv_fechar(Csv *csv);

#endif /* CSV_H */
