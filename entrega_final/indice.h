/*
 * indice.h - TAD Índice Primário.
 *
 * O arquivo de índice primário é leve (apenas codEstacao + RRN por entrada)
 * e por isso é carregado inteiro para um vetor em RAM no início das
 * operações, manipulado dinamicamente (permitindo busca binária) e
 * reescrito em disco ao final.
 *
 * Layout em disco:
 *   [status 1 byte]['1' consistente | '0' inconsistente]
 *   [codEstacao 4][RRN 4] [codEstacao 4][RRN 4] ...
 * As entradas ficam SEMPRE ordenadas de forma crescente por codEstacao.
 */

#ifndef INDICE_H
#define INDICE_H

#include <stdio.h>

/* Tamanhos em bytes do índice */
#define TAM_INDICE_STATUS    1
#define TAM_INDICE_COD       4
#define TAM_INDICE_RRN       4
#define TAM_INDICE_ENTRADA   8   /* codEstacao(4) + RRN(4) */

/* Valores de status (iguais aos do cabeçalho de dados) */
#define INDICE_STATUS_INCONSISTENTE '0'
#define INDICE_STATUS_CONSISTENTE   '1'

/* Códigos de retorno */
typedef enum {
    INDICE_OK   =  1,
    INDICE_EOF  =  0,
    INDICE_ERRO = -1
} IndiceStatus;

/* Uma entrada do índice: chave primária + posição no arquivo de dados */
typedef struct {
    int codEstacao;
    int rrn;
} IndiceEntrada;

/* Índice carregado em memória */
typedef struct {
    IndiceEntrada *itens;
    int  n;          /* quantidade de entradas em uso */
    int  cap;        /* capacidade alocada */
    char status;     /* status lido do arquivo */
} Indice;

/* Inicializa um índice vazio (sem alocação). */
void indice_start(Indice *idx);

/* Libera a memória do vetor de entradas. */
void indice_free(Indice *idx);

/*
 * Carrega o índice do arquivo para a RAM.
 * Retorna INDICE_ERRO se não abrir ou se o status estiver inconsistente.
 */
IndiceStatus indice_carregar(Indice *idx, const char *caminho);

/*
 * Grava o índice em disco (status '1' + entradas em ordem crescente).
 * Sobrescreve o arquivo.
 */
IndiceStatus indice_salvar(const Indice *idx, const char *caminho);

/*
 * Busca binária por codEstacao.
 * Retorna o RRN correspondente, ou -1 se não encontrado.
 */
int indice_buscar(const Indice *idx, int codEstacao);

/*
 * Insere (codEstacao, rrn) mantendo a ordem crescente por codEstacao.
 * Retorna INDICE_OK ou INDICE_ERRO (falha de alocação).
 */
IndiceStatus indice_inserir(Indice *idx, int codEstacao, int rrn);

/*
 * Remove a entrada de um dado codEstacao (deslocando as seguintes).
 * Retorna INDICE_OK se removeu, INDICE_EOF se não existia.
 */
IndiceStatus indice_remover(Indice *idx, int codEstacao);

#endif /* INDICE_H */
