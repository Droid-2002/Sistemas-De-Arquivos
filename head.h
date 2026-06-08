/*
 * NUSP: XXXXXXXX - Nome do Aluno 1
 * NUSP: YYYYYYYY - Nome do Aluno 2
 *
 * SCC0503 - Algoritmos e Estruturas de Dados II
 * Trabalho Prático 1 - Sistema de Arquivos Binários
 * ICMC - USP São Carlos
 *
 * head.h - TAD Cabeçalho: definição da struct e protótipos das funções
 *          que lêem e escrevem o registro de cabeçalho de 17 bytes.
 */

#ifndef HEAD_H
#define HEAD_H

#include <stdio.h>

/* Tamanhos em bytes de cada campo do cabeçalho */
#define TAM_CABECALHO       17
#define TAM_STATUS           1
#define TAM_TOPO             4
#define TAM_PROX_RRN         4
#define TAM_NRO_ESTACOES     4
#define TAM_NRO_PARES_ESTACAO 4

/* Valores válidos para o campo status */
#define STATUS_INCONSISTENTE '0'   /* arquivo aberto para escrita / interrompido */
#define STATUS_CONSISTENTE   '1'   /* arquivo fechado corretamente */

/* Enum para identificar campos ao usar os setters */
typedef enum {
    HEAD_CAMPO_STATUS,
    HEAD_CAMPO_TOPO,
    HEAD_CAMPO_PROX_RRN,
    HEAD_CAMPO_NRO_ESTACOES,
    HEAD_CAMPO_NRO_PARES_ESTACAO
} HeadCampo;

/* Código de retorno das funções do cabeçalho */
typedef enum {
    HEAD_OK   =  1,
    HEAD_EOF  =  0,
    HEAD_ERRO = -1
} HeadStatus;

/* Struct que representa o cabeçalho em memória */
typedef struct {
    char status;          /* '0' inconsistente, '1' consistente             */
    int  topo;            /* RRN do primeiro registro removido (-1 = vazio) */
    int  proxRRN;         /* próximo RRN disponível para inserção            */
    int  nroEstacoes;     /* quantidade de estações distintas (por nome)     */
    int  nroParesEstacao; /* quantidade de pares (codEstacao, codProxEstacao)*/
} Head;

/* Inicializa o cabeçalho com valores padrão (status='0', topo=-1, demais=0) */
void head_start(Head *h);

/* Escreve o cabeçalho no início do arquivo (fseek para byte 0 antes de gravar) */
HeadStatus head_escrever(FILE *fp, const Head *h);

/* Lê o cabeçalho do início do arquivo (fseek para byte 0 antes de ler) */
HeadStatus head_ler(FILE *fp, Head *h);

/* Setters para campos individuais do cabeçalho */
HeadStatus head_set_char(Head *h, HeadCampo campo, char valor);
HeadStatus head_set_int(Head *h, HeadCampo campo, int valor);

#endif /* HEAD_H */
