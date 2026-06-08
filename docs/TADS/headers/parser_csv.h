#ifndef PARSER_CSV_H
#define PARSER_CSV_H

#include <stdio.h>
#include "TADS/headers/registro.h"

// tamanho máximo de uma linha do CSV
#define MAX_LINHA_CSV 1024

// indicador de campo vazio
#define CAMPO_VAZIO ""

typedef enum {
    CSV_OK = 1,
    CSV_EOF = 0,
    CSV_ERRO = -1
} CsvStatus;

typedef struct {
    FILE *fp;
    int linha_atual;
} Csv;

/*
 *  CsvStatus csv_abrir(Csv *csv, const char *caminho);
 *  Abre um arquivo CSV para leitura e posiciona no início.
 *
 *  Args:
 *      csv: ponteiro para a estrutura de controle do CSV
 *      caminho: string com o caminho do arquivo CSV
 *
 *  Retorna:
 *      CSV_OK (1) se o arquivo foi aberto com sucesso
 *      CSV_ERRO (-1) se ocorreu erro ao abrir o arquivo
 */
CsvStatus csv_abrir(Csv *csv, const char *caminho);

/*
 *  CsvStatus csv_pular_cabecalho(Csv *csv);
 *  Lê e descarta a primeira linha do arquivo CSV (linha de cabeçalho).
 *
 *  Args:
 *      csv: ponteiro para a estrutura de controle do CSV
 *
 *  Retorna:
 *      CSV_OK (1) se a linha foi descartada com sucesso
 *      CSV_EOF (0) se o arquivo está vazio
 *      CSV_ERRO (-1) se ocorreu erro de leitura
 */
CsvStatus csv_pular_cabecalho(Csv *csv);


/*
 *  CsvStatus csv_ler_registro(Csv *csv, Registro *r);
 *  Lê a próxima linha do CSV, faz o parsing dos 8 campos e preenche um Registro.
 *  Trata campos vazios como nulos (valor -1 para inteiros, string vazia para texto).
 *
 *  Args:
 *      csv: ponteiro para a estrutura de controle do CSV
 *      r: ponteiro para o Registro a ser preenchido
 *
 *  Retorna:
 *      CSV_OK (1) se a linha foi lida e parseada com sucesso
 *      CSV_EOF (0) se chegou ao fim do arquivo
 *      CSV_ERRO (-1) se ocorreu erro de leitura ou parse inválido
 */
CsvStatus csv_ler_registro(Csv *csv, Registro *r);

/*
 *  void csv_fechar(Csv *csv);
 *  Fecha o arquivo CSV e libera recursos associados.
 *
 *  Args:
 *      csv: ponteiro para a estrutura de controle do CSV
 *
 *  Retorna:
 *      Nenhum valor retornado
 */
void csv_fechar(Csv *csv);


#endif // PARSER_CSV_H