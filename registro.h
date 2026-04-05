/*
 * registro.h - TAD Registro
 *
 * Cada registro ocupa exatamente 80 bytes no arquivo binario.
 * Campos de tamanho variavel (nomeEstacao, nomeLinha) sao
 * armazenados sem '\0', e o espaco restante eh preenchido com '$'.
 */

#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdio.h>

/* codigos de retorno */
typedef enum {
    REG_OK   =  1,
    REG_EOF  =  0,
    REG_ERRO = -1
} RegistroStatus;

/* struct que guarda os dados de um registro em memoria */
typedef struct {
    char removido;          /* '0' = ativo, '1' = removido */
    int  proximo;           /* RRN do prox removido (-1 se nao tem) */

    int  codEstacao;
    int  codLinha;
    int  codProxEstacao;
    int  distProxEstacao;
    int  codLinhaIntegra;
    int  codEstIntegra;

    int  tamNomeEstacao;    /* tamanho da string (sem \0) */
    char *nomeEstacao;

    int  tamNomeLinha;      /* 0 = nulo */
    char *nomeLinha;
} Registro;

/* inicializa com valores padrao (inteiros = -1, strings = NULL) */
Registro registro_start(void);

/* libera memoria alocada para as strings */
void registro_free(Registro *r);

/* setters para os campos string - fazem copia interna */
RegistroStatus registro_set_nome_estacao(Registro *r, const char *nome, int tam);
RegistroStatus registro_set_nome_linha(Registro *r, const char *nome, int tam);

/* calcula bytes de lixo '$' pra completar 80 bytes */
int registro_bytes_lixo(const Registro *r);

/* offset no arquivo pra um dado RRN */
long registro_rrn_offset(int rrn);

/* escrita e leitura campo a campo no binario */
RegistroStatus registro_escrever_bin(FILE *fp, const Registro *r);
RegistroStatus registro_ler_bin(FILE *fp, Registro *r);

/* imprime no formato pedido pela especificacao */
void registro_imprimir(const Registro *r);

/* retorna 1 se o registro bate com o criterio (campo, valor) */
int registro_match(const Registro *r, const char *campo, const char *valor);

#endif
