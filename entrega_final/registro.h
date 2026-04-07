#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdio.h>

/* codigos de retorno */
typedef enum registro_status {
    REG_OK  =  1,
    REG_EOF =  0,
    REG_ERRO = -1
} RegistroStatus;

/* enumeracao para acesso generico aos campos do registro */
typedef enum registro_campo {
    REG_CAMPO_REMOVIDO,
    REG_CAMPO_PROXIMO,
    REG_CAMPO_COD_ESTACAO,
    REG_CAMPO_COD_LINHA,
    REG_CAMPO_COD_PROX_ESTACAO,
    REG_CAMPO_DIST_PROX_ESTACAO,
    REG_CAMPO_COD_LINHA_INTEGRA,
    REG_CAMPO_COD_EST_INTEGRA,
    REG_CAMPO_NOME_ESTACAO,
    REG_CAMPO_NOME_LINHA
} RegistroCampo;

/* struct que guarda os dados de um registro em memoria */
typedef struct {
    char removido;          /* '0' = ativo, '1' = removido */
    int  proximo;           /* RRN do prox removido (-1 se nao tem) */

    int  codEstacao;        /* obrigatorio */
    int  codLinha;          /* nulo = -1 */
    int  codProxEstacao;    /* nulo = -1 */
    int  distProxEstacao;   /* nulo = -1 */
    int  codLinhaIntegra;   /* nulo = -1 */
    int  codEstIntegra;     /* nulo = -1 */

    int  tamNomeEstacao;    /* tamanho da string (sem \0), >= 1 */
    char *nomeEstacao;

    int  tamNomeLinha;      /* 0 = nulo/vazio, >=1 = tamanho da string */
    char *nomeLinha;        /* nome vazio = nulo */
} Registro;

/* inicializa com valores padrao (inteiros = -1, strings = NULL) */
Registro registro_start(void);

/* libera memoria alocada para as strings */
void registro_free(Registro *r);

/* setters seguros para strings - fazem copia interna */
RegistroStatus registro_set_nome_estacao(Registro *r, const char *nome, int tam_nome);
RegistroStatus registro_set_nome_linha(Registro *r, const char *nome, int tam_nome);

/* calcula bytes de lixo '$' pra completar 80 bytes */
int registro_bytes_lixo(const Registro *r);

/* escrita e leitura campo a campo no binario */
RegistroStatus registro_escrever_bin(FILE *fp, const Registro *r);
RegistroStatus registro_ler_bin(FILE *fp, Registro *r);

/* impressao no formato pedido pela especificacao */
void registro_imprimir(const Registro *r);

/* offset no arquivo pra um dado RRN */
long registro_rrn_offset(int rrn);

/* setters genericos por tipo/campo */
RegistroStatus registro_set_char(Registro *r, RegistroCampo campo, char valor);
RegistroStatus registro_set_int(Registro *r, RegistroCampo campo, int valor);
RegistroStatus registro_set_str(Registro *r, RegistroCampo campo, const char *valor);

/* retorna 1 se o registro bate com o criterio (campo, valor) */
int registro_match(const Registro *r, const char *campo, const char *valor);

#endif /* REGISTRO_H */
