/*
 * NUSP: XXXXXXXX - Nome do Aluno 1
 * NUSP: YYYYYYYY - Nome do Aluno 2
 *
 * SCC0503 - Algoritmos e Estruturas de Dados II
 * Trabalho Prático 1 - Sistema de Arquivos Binários de Metrô/CPTM
 * Instituto de Ciências Matemáticas e de Computação - USP São Carlos
 *
 * main.c - Ponto de entrada do programa.
 *
 * Lê o identificador da funcionalidade na primeira linha da entrada,
 * depois os argumentos necessários, e despacha para a função correta.
 *
 * Funcionalidades:
 *   1 arquivo.csv arquivo.bin   → CREATE TABLE (CSV → binário)
 *   2 arquivo.bin               → SELECT * (listar todos)
 *   3 arquivo.bin n             → SELECT WHERE (busca por campo, n consultas)
 *   4 arquivo.bin RRN           → SELECT por RRN (acesso direto)
 */

#include <stdio.h>
#include <stdlib.h>
#include "funcionalidades.h"

int main(void) {
    int funcionalidade;

    /* ler o número da funcionalidade desejada */
    if (scanf("%d", &funcionalidade) != 1) {
        fprintf(stderr, "Erro: não foi possível ler a funcionalidade.\n");
        return 1;
    }

    char arquivo_csv[512];
    char arquivo_bin[512];
    int  rrn;
    int  n;

    switch (funcionalidade) {

        /* --- Funcionalidade 1: CSV → binário --- */
        case 1:
            scanf("%s %s", arquivo_csv, arquivo_bin);
            func_criar_tabela(arquivo_csv, arquivo_bin);
            break;

        /* --- Funcionalidade 2: SELECT * --- */
        case 2:
            scanf("%s", arquivo_bin);
            func_select_todos(arquivo_bin);
            break;

        /* --- Funcionalidade 3: SELECT WHERE (n consultas) --- */
        case 3:
            scanf("%s %d", arquivo_bin, &n);
            func_select_where(arquivo_bin, n);
            break;

        /* --- Funcionalidade 4: SELECT por RRN --- */
        case 4:
            scanf("%s %d", arquivo_bin, &rrn);
            func_select_rrn(arquivo_bin, rrn);
            break;

        default:
            fprintf(stderr, "Funcionalidade desconhecida: %d\n", funcionalidade);
            return 1;
    }

    return 0;
}
