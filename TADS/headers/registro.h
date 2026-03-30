#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdio.h>


// Definições de tamanhos e constantes para o registro
#define TAM_REGISTRO 80

#define TAM_REMOVIDO            1
#define TAM_PROXIMO             4
#define TAM_COD_ESTACAO         4
#define TAM_COD_LINHA           4
#define TAM_COD_PROX_ESTACAO    4
#define TAM_DIST_PROX_ESTACAO   4
#define TAM_COD_LINHA_INTEGRA   4
#define TAM_COD_EST_INTEGRA     4
#define TAM_INDICE_NOME_ESTACAO 4
#define TAM_INDICE_NOME_LINHA   4

#define BYTES_FIXOS_REGISTRO ( \
    TAM_REMOVIDO + TAM_PROXIMO + TAM_COD_ESTACAO + TAM_COD_LINHA + \
    TAM_COD_PROX_ESTACAO + TAM_DIST_PROX_ESTACAO + TAM_COD_LINHA_INTEGRA + \
    TAM_COD_EST_INTEGRA + TAM_INDICE_NOME_ESTACAO + TAM_INDICE_NOME_LINHA \
)

#define BYTES_BASE_LIXO (TAM_REGISTRO - BYTES_FIXOS_REGISTRO)

typedef enum {
    REG_OK = 1,
    REG_EOF = 0,
    REG_ERRO = -1
} RegistroStatus;

typedef enum {
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


typedef struct {
    char removido;                // '0' ativo, '1' removido
    int proximo;                  // encadeamento da lista de removidos

    int codEstacao;               // obrigatório
    int codLinha;                 // nulo = -1
    int codProxEstacao;           // nulo = -1
    int distProxEstacao;          // nulo = -1
    int codLinhaIntegra;          // nulo = -1
    int codEstIntegra;            // nulo = -1

    int tamNomeEstacao;           // >= 1
    char *nomeEstacao; 

    int tamNomeLinha;             // 0 = nulo
    char *nomeLinha;     
} Registro;


// Métodos do Registro

/*
*   void registro_start(Registro *r);
*   Inicializa os campos de um registro,
*   
*   Args:
*       r: ponteiro para o registro a ser inicializado
*   Retorna:
*       Nenhum valor retornado
*       Uso de ponteiros
*/
void registro_start(Registro *r);

/*
*   void registro_free(Registro *r);
*   Libera a memória alocada para os campos de um registro,
*   e para o próprio registro. ->  não deixa ter vazamento de memória
*
*   Args:
*       r: ponteiro para o registro que deve ser liberado
*   Retorna:
*       Nenhum valor retornado.
*/
void registro_free(Registro *r);   

// Como vamos usar alocação dinamica para definir o char, vamos usar setter para encapsular e ter segurança na ação
/* setters seguros para strings (fazem cópia interna) */

/*
*   int registro_set_nome_estacao(Registro *r, const char *nome);
*   Define o nome da estação em um registro, alocando memória para armazenar 
*   a string e copiando o conteúdo do nome fornecido.
*
*   Args:
*       r: ponteiro para o registro onde o nome da estação deve ser definido
*       nome: string contendo o nome da estação a ser definida
*   Retorna:
*       REG_OK (1) se o nome foi definido com sucesso
*       REG_ERRO (-1) se ocorreu um erro durante a definição do nome (ex: falha na alocação de memória)
*/
int registro_set_nome_estacao(Registro *r, const char *nome);

/*

*   RegistroStatus registro_set_nome_linha(Registro *r, const char *nome);
*   Define o nome da linha em um registro, alocando memória para armazenar
*   a string e copiando o conteúdo do nome fornecido.
*   Args:
*       r: ponteiro para o registro onde o nome da linha deve ser definido
*       nome: string contendo o nome da linha a ser definida
*   Retorna:
*       REG_OK (1) se o nome foi definido com sucesso
*       REG_ERRO (-1) se ocorreu um erro durante a definição do nome (ex: falha na alocação de memória)
*/
RegistroStatus registro_set_nome_linha(Registro *r, const char *nome); /* nome vazio => nulo */

/*
*   int registro_bytes_lixo(const Registro *r);
*   Calcula a quantidade de bytes de lixo em um registro, considerando o tamanho total do
*   registro e os bytes ocupados pelos campos fixos e variáveis.
*   Args:
*       r: ponteiro para o registro para o qual a quantidade de bytes de lixo deve ser calculada
*   Retorna:
*       A quantidade de bytes de lixo no registro.
*/
int registro_bytes_lixo(const Registro *r);

/*
*   RegistroStatus registro_escrever_bin(FILE *fp, const Registro *r);
*   Escreve um registro em um arquivo binário, seguindo o formato definido para os
*   campos do registro. O registro é escrito de forma compacta, utilizando os bytes
*   necessários para armazenar os campos, e preenchendo os bytes restantes com lixo.
*   Args:
*       fp: ponteiro para o arquivo binário onde o registro deve ser escrito (deve estar aberto para escrita)
*       r: ponteiro para o registro que deve ser escrito no arquivo
*   Retorna:
*       REG_OK (1) se o registro foi escrito com sucesso
*       REG_ERRO (-1) se ocorreu um erro durante a escrita do registro (ex: falha na escrita no arquivo)
*/
RegistroStatus registro_escrever_bin(FILE *fp, const Registro *r);

/*
*   RegistroStatus registro_ler_bin(FILE *fp, Registro *r);
*   Lê um registro de um arquivo binário, seguindo o formato definido para os
*   campos do registro.
*   Args:
*       fp: ponteiro para o arquivo binário de onde o registro deve ser lido (deve estar aberto para leitura)
*       r: ponteiro para o registro onde os dados lidos devem ser armazenados
*   Retorna:
*       REG_OK (1) se o registro foi lido com sucesso
*       REG_ERRO (-1) se ocorreu um erro durante a leitura do registro (ex: falha na leitura do arquivo)
*/
RegistroStatus registro_ler_bin(FILE *fp, Registro *r);

/*
*   void registro_imprimir(const Registro *r);
*   Imprime os campos de um registro.
*   Args:
*       r: ponteiro para o registro que deve ser impresso
*   Retorna:
*       Nenhum valor retornado.
*       Imprime na tela os campos do regsitro.
*/
void registro_imprimir(const Registro *r);

/*
*   long registro_rrn_offset(int rrn);
*   Calcula o deslocamento (offset) em bytes para um registro 
*   com um determinado RRN (Relative Record Number) em um arquivo binário, 
*    considerando o tamanho fixo de cada registro.
*   Args:
*       rrn: número de registro relativo
*   Retorna:
*       O deslocamento em bytes para o registro com o RRN especificado.
*/
long registro_rrn_offset(int rrn);

/*
*   RegistroStatus registro_set_char(Registro *r, RegistroCampo campo, char valor);
*   RegistroStatus registro_set_int(Registro *r, RegistroCampo campo, int valor);
*   RegistroStatus registro_set_str(Registro *r, RegistroCampo campo, const char *valor);
*   Define o valor de um campo específico em um registro.
*   Args:
*       r: ponteiro para o registro onde o campo deve ser definido
*       campo: enumeração indicando qual campo do registro deve ser definido
*       valor: valor a ser atribuído ao campo (char para campos de caractere, int para campos inteiros, const char* para campos de string)
*   Retorna:
*       REG_OK (1) se o campo foi definido com sucesso
*       REG_ERRO (-1) se ocorreu um erro durante a definição do campo
*/
RegistroStatus registro_set_char(Registro *r, RegistroCampo campo, char valor);
RegistroStatus registro_set_int(Registro *r, RegistroCampo campo, int valor);
RegistroStatus registro_set_str(Registro *r, RegistroCampo campo, const char *valor);


#endif // REGISTRO_H