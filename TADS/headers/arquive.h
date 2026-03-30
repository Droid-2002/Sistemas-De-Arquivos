#ifndef ARQUIVE_H
#define ARQUIVE_H

#include <stdio.h>
#include "TADS/headers/registro.h"
#include "TADS/headers/head.h"

typedef enum{
    ARQ_OK = 1,
    ARQ_EOF = 0,
    ARQ_ERRO = -1
} ArqStatus;

typedef enum {
    ARQ_MODO_LEITURA = 0,
    ARQ_MODO_ESCRITA = 1
} ArqModo;

typedef struct {
    FILE *fp;
    Head head;
    ArqModo modo; // 0 leitura e 1 escrita/atualização
    int rrn_atual; // RRN do registro atualmente lido ou escrito
} Arquivo;

/*
 *  ArqStatus arquivo_abrir(Arquivo *arq, const char *caminho, const char *modo);
 *  Abre um arquivo binário no modo informado e inicializa a estrutura de controle.
 *
 *  Args:
 *      arq: ponteiro para a estrutura de controle do arquivo
 *      caminho: string com o caminho do arquivo binário
 *      modo: modo de abertura do C (ex: "rb", "wb", "r+b")
 *
 *  Retorna:
 *      ARQ_OK (1) se o arquivo foi aberto com sucesso
 *      ARQ_ERRO (-1) se ocorreu erro ao abrir o arquivo
 */
ArqStatus arquivo_abrir(Arquivo *arq, const char *caminho, const char *modo);

/*
 *  ArqStatus arquivo_fechar(Arquivo *arq);
 *  Fecha o arquivo binário associado e limpa o controle básico da estrutura.
 *
 *  Args:
 *      arq: ponteiro para a estrutura de controle do arquivo
 *
 *  Retorna:
 *      ARQ_OK (1) se o arquivo foi fechado com sucesso
 *      ARQ_ERRO (-1) se ocorreu erro ao fechar o arquivo
 */
ArqStatus arquivo_fechar(Arquivo *arq);

/*
 *  ArqStatus arquivo_ler_head(Arquivo *arq);
 *  Lê o cabeçalho do arquivo binário e armazena os dados em arq->head.
 *
 *  Args:
 *      arq: ponteiro para a estrutura de controle do arquivo
 *
 *  Retorna:
 *      ARQ_OK (1) se o cabeçalho foi lido com sucesso
 *      ARQ_EOF (0) se o arquivo não possui cabeçalho completo
 *      ARQ_ERRO (-1) se ocorreu erro de leitura
 */
ArqStatus arquivo_ler_head(Arquivo *arq);

/*
 *  ArqStatus arquivo_escrever_head(Arquivo *arq);
 *  Escreve o conteúdo atual de arq->head no início do arquivo binário.
 *
 *  Args:
 *      arq: ponteiro para a estrutura de controle do arquivo
 *
 *  Retorna:
 *      ARQ_OK (1) se o cabeçalho foi escrito com sucesso
 *      ARQ_ERRO (-1) se ocorreu erro de escrita
 */
ArqStatus arquivo_escrever_head(Arquivo *arq);

/*
 *  ArqStatus arquivo_marcar_inconsistente(Arquivo *arq);
 *  Atualiza o status do cabeçalho para inconsistente ('0') em memória e em disco.
 *
 *  Args:
 *      arq: ponteiro para a estrutura de controle do arquivo
 *
 *  Retorna:
 *      ARQ_OK (1) se o status foi atualizado com sucesso
 *      ARQ_ERRO (-1) se ocorreu erro na atualização
 */
ArqStatus arquivo_marcar_inconsistente(Arquivo *arq);

/*
 *  ArqStatus arquivo_marcar_consistente(Arquivo *arq);
 *  Atualiza o status do cabeçalho para consistente ('1') em memória e em disco.
 *
 *  Args:
 *      arq: ponteiro para a estrutura de controle do arquivo
 *
 *  Retorna:
 *      ARQ_OK (1) se o status foi atualizado com sucesso
 *      ARQ_ERRO (-1) se ocorreu erro na atualização
 */
ArqStatus arquivo_marcar_consistente(Arquivo *arq);

/*
 *  ArqStatus arquivo_append_registro(Arquivo *arq, const Registro *r);
 *  Escreve um registro no fim lógico do arquivo (append), atualizando os
 *  metadados necessários em arq->head.
 *
 *  Args:
 *      arq: ponteiro para a estrutura de controle do arquivo
 *      r: ponteiro para o registro a ser escrito
 *
 *  Retorna:
 *      ARQ_OK (1) se o registro foi escrito com sucesso
 *      ARQ_ERRO (-1) se ocorreu erro de escrita
 */
ArqStatus arquivo_append_registro(Arquivo *arq, const Registro *r);


/*
 *  ArqStatus arquivo_ler_rrn(Arquivo *arq, int rrn, Registro *r);
 *  Lê um registro específico pelo RRN informado.
 *
 *  Args:
 *      arq: ponteiro para a estrutura de controle do arquivo
 *      rrn: número relativo do registro desejado
 *      r: ponteiro para o registro que receberá os dados lidos
 *
 *  Retorna:
 *      ARQ_OK (1) se o registro foi lido com sucesso
 *      ARQ_EOF (0) se o RRN está fora da área válida de dados
 *      ARQ_ERRO (-1) se ocorreu erro de leitura/posicionamento
 */
ArqStatus arquivo_ler_rrn(Arquivo *arq, int rrn, Registro *r);


#endif // ARQUIVE_H