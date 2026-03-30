#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>

/* Layout do cabeçalho em disco */
#define TAM_CABECALHO 17

#define TAM_STATUS 1
#define TAM_TOPO 4
#define TAM_PROX_RRN 4
#define TAM_NRO_ESTACOES 4
#define TAM_NRO_PARES_ESTACAO 4

/* Valores do status */
#define STATUS_INCONSISTENTE '0'
#define STATUS_CONSISTENTE '1'

typedef enum {
HEAD_CAMPO_STATUS,
HEAD_CAMPO_TOPO,
HEAD_CAMPO_PROX_RRN,
HEAD_CAMPO_NRO_ESTACOES,
HEAD_CAMPO_NRO_PARES_ESTACAO
} HeadCampo;

typedef enum {
HEAD_OK = 1,
HEAD_EOF = 0,
HEAD_ERRO = -1
} HeadStatus;

typedef struct {
    char status;            // '0' inconsistente, '1' consistente
    int topo;              // RRN do topo da pilha de removidos
    int proxRRN;           // RRN do próximo registro a ser inserido
    int nroEstacoes;       // número total de estações cadastradas
    int nroParesEstacao;   // número total de pares estação-linha cadastrados
} Head;

/*
 void head_start(Head *h);
 Inicializa os campos de um cabeçalho,
 definindo o status como inconsistente e os demais campos como 0.
 Args:
    h: ponteiro para o cabeçalho a ser inicializado
 Retorna:
    Nenhum valor retornado
*/
void head_start(Head *h);

/*
    HeadStatus head_escrever(FILE *fp, const Head *h);
    Escreve os campos de um cabeçalho em um arquivo binário,
    seguindo o formato definido para os arquivos de dados.
    Args:
        fp: ponteiro para o arquivo binário onde o cabeçalho deve ser escrito
        h: ponteiro para o cabeçalho a ser escrito
    Retorna:
        HEAD_OK (1) se o cabeçalho foi escrito com sucesso
        HEAD_ERRO (-1) se ocorreu um erro durante a escrita do cabeçalho

*/
HeadStatus head_escrever(FILE *fp, const Head *h);


/*
    HeadStatus head_ler(FILE *fp, Head *h);
    Lê os campos de um cabeçalho de um arquivo binário,
    seguindo o formato definido para os arquivos de dados.
    Args:
        fp: ponteiro para o arquivo binário de onde o cabeçalho deve ser lido
        h: ponteiro para o cabeçalho onde os dados lidos devem ser armazenados
    Retorna:
        HEAD_OK (1) se o cabeçalho foi lido com sucesso
        HEAD_EOF (0) se o final do arquivo foi alcançado durante a leitura do cabeçalho
        HEAD_ERRO (-1) se ocorreu um erro durante a leitura do cabeçalho
*/
HeadStatus head_ler(FILE *fp, Head *h);

/*
    HeadStatus head_set_char(Head *h, HeadCampo campo, char valor);
    HeadStatus head_set_int(Head *h, HeadCampo campo, int valor);
    Define o valor de um campo específico em um cabeçalho.
    Args:
        h: ponteiro para o cabeçalho onde o campo deve ser definido
        campo: enumeração indicando qual campo deve ser definido (status, topo, proxRRN, nroEstacoes ou nroParesEstacao)
        valor: valor a ser atribuído ao campo (char para status, int para os demais campos)
    Retorna:
        HEAD_OK (1) se o campo foi definido com sucesso
        HEAD_ERRO (-1) se ocorreu um erro durante a definição do campo (ex: campo inválido)
*/
HeadStatus head_set_char(Head *h, HeadCampo campo, char valor);
HeadStatus head_set_int(Head *h, HeadCampo campo, int valor);

#endif // HEAD_H