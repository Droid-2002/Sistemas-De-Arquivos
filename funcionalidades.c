/*
 * funcionalidades.c - Implementação das 4 funcionalidades do trabalho.
 *
 * Cada função orquestra os TADs (Head, Registro, CSV) para executar
 * uma operação específica sobre o arquivo binário.
 */

#include "funcionalidades.h"
#include "head.h"
#include "registro.h"
#include "csv.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -----------------------------------------------------------------------
 * Estruturas auxiliares para contar estações e pares distintos
 * (usadas apenas na funcionalidade [1] - CREATE TABLE)
 * ----------------------------------------------------------------------- */

/* Lista dinâmica de strings para contar nomes de estações distintos */
typedef struct {
    char **nomes;
    int    count;
    int    capacidade;
} ListaNomes;

static ListaNomes lista_nomes_criar(void) {
    ListaNomes l;
    l.capacidade = 32;
    l.count      = 0;
    l.nomes      = (char **)malloc(l.capacidade * sizeof(char *));
    return l;
}

/* Retorna 1 se o nome já existe na lista, 0 caso contrário */
static int lista_nomes_contem(const ListaNomes *l, const char *nome) {
    for (int i = 0; i < l->count; i++) {
        if (strcmp(l->nomes[i], nome) == 0) return 1;
    }
    return 0;
}

/* Insere um nome na lista (assume que já verificou que não existe) */
static void lista_nomes_inserir(ListaNomes *l, const char *nome) {
    if (l->count == l->capacidade) {
        l->capacidade *= 2;
        l->nomes = (char **)realloc(l->nomes, l->capacidade * sizeof(char *));
    }
    l->nomes[l->count] = (char *)malloc(strlen(nome) + 1);
    strcpy(l->nomes[l->count], nome);
    l->count++;
}

static void lista_nomes_destruir(ListaNomes *l) {
    for (int i = 0; i < l->count; i++) free(l->nomes[i]);
    free(l->nomes);
    l->nomes = NULL;
    l->count = 0;
}

/* Lista dinâmica de pares (codEstacao, codProxEstacao) */
typedef struct { int cod; int prox; } Par;

typedef struct {
    Par *pares;
    int  count;
    int  capacidade;
} ListaPares;

static ListaPares lista_pares_criar(void) {
    ListaPares l;
    l.capacidade = 32;
    l.count      = 0;
    l.pares      = (Par *)malloc(l.capacidade * sizeof(Par));
    return l;
}

/* Retorna 1 se o par já existe, 0 caso contrário */
static int lista_pares_contem(const ListaPares *l, int cod, int prox) {
    for (int i = 0; i < l->count; i++) {
        if (l->pares[i].cod == cod && l->pares[i].prox == prox) return 1;
    }
    return 0;
}

static void lista_pares_inserir(ListaPares *l, int cod, int prox) {
    if (l->count == l->capacidade) {
        l->capacidade *= 2;
        l->pares = (Par *)realloc(l->pares, l->capacidade * sizeof(Par));
    }
    l->pares[l->count].cod  = cod;
    l->pares[l->count].prox = prox;
    l->count++;
}

static void lista_pares_destruir(ListaPares *l) {
    free(l->pares);
    l->pares = NULL;
    l->count = 0;
}

/* -----------------------------------------------------------------------
 * Funcionalidade [1] - CREATE TABLE: CSV → binário
 * ----------------------------------------------------------------------- */

void func_criar_tabela(const char *arquivo_csv, const char *arquivo_bin) {
    /* --- abrir CSV --- */
    Csv csv;
    if (csv_abrir(&csv, arquivo_csv) != CSV_OK) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }
    if (csv_pular_cabecalho(&csv) != CSV_OK) {
        printf("Falha no processamento do arquivo.\n");
        csv_fechar(&csv);
        return;
    }

    /* --- criar arquivo binário --- */
    FILE *fp = fopen(arquivo_bin, "wb");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        csv_fechar(&csv);
        return;
    }

    /* --- escrever cabeçalho inicial ---
     * status='0' indica arquivo inconsistente enquanto a escrita ocorre.
     * Será atualizado para '1' ao final. */
    Head head;
    head_start(&head);
    if (head_escrever(fp, &head) != HEAD_OK) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        csv_fechar(&csv);
        return;
    }

    /* --- estruturas auxiliares para contar distintos --- */
    ListaNomes nomes_vistos  = lista_nomes_criar();
    ListaPares pares_vistos  = lista_pares_criar();

    /* --- processar cada linha do CSV --- */
    Registro r;
    CsvStatus csv_status;

    while ((csv_status = csv_ler_registro(&csv, &r)) == CSV_OK) {
        /* gravar registro no binário */
        if (registro_escrever_bin(fp, &r) != REG_OK) {
            printf("Falha no processamento do arquivo.\n");
            registro_free(&r);
            lista_nomes_destruir(&nomes_vistos);
            lista_pares_destruir(&pares_vistos);
            fclose(fp);
            csv_fechar(&csv);
            return;
        }

        /* atualizar proxRRN */
        head.proxRRN++;

        /* contar nomes de estações distintos pelo nomeEstacao */
        if (r.nomeEstacao != NULL && !lista_nomes_contem(&nomes_vistos, r.nomeEstacao)) {
            lista_nomes_inserir(&nomes_vistos, r.nomeEstacao);
            head.nroEstacoes++;
        }

        /* contar pares (codEstacao, codProxEstacao) distintos onde codProxEstacao != -1 */
        if (r.codProxEstacao != -1) {
            if (!lista_pares_contem(&pares_vistos, r.codEstacao, r.codProxEstacao)) {
                lista_pares_inserir(&pares_vistos, r.codEstacao, r.codProxEstacao);
                head.nroParesEstacao++;
            }
        }

        registro_free(&r);
    }

    /* liberar auxiliares */
    lista_nomes_destruir(&nomes_vistos);
    lista_pares_destruir(&pares_vistos);
    csv_fechar(&csv);

    /* --- atualizar cabeçalho final com os contadores e status='1' --- */
    head.status = STATUS_CONSISTENTE;
    if (head_escrever(fp, &head) != HEAD_OK) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    /* fechar arquivo ANTES de chamar BinarioNaTela */
    fclose(fp);

    /* exibir conteúdo binário (obrigatório conforme especificação) */
    BinarioNaTela((char *)arquivo_bin);
}

/* -----------------------------------------------------------------------
 * Funcionalidade [2] - SELECT *: listar todos os registros ativos
 * ----------------------------------------------------------------------- */

void func_select_todos(const char *arquivo_bin) {
    FILE *fp = fopen(arquivo_bin, "rb");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* ler e verificar cabeçalho */
    Head head;
    if (head_ler(fp, &head) != HEAD_OK || head.status == STATUS_INCONSISTENTE) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    /* percorrer registros sequencialmente após o cabeçalho */
    int encontrou = 0;
    Registro r;
    RegistroStatus reg_status;

    while ((reg_status = registro_ler_bin(fp, &r)) == REG_OK) {
        if (r.removido == '0') {
            registro_imprimir(&r);
            encontrou = 1;
        }
        registro_free(&r);
    }

    if (!encontrou) {
        printf("Registro inexistente.\n");
    }

    fclose(fp);
}

/* -----------------------------------------------------------------------
 * Funcionalidade [3] - SELECT WHERE: busca por critérios de campo
 * ----------------------------------------------------------------------- */

/* Tamanhos máximos para os buffers de campo e valor na busca */
#define MAX_NOME_CAMPO  64
#define MAX_VALOR_CAMPO 256
#define MAX_CRITERIOS   20

void func_select_where(const char *arquivo_bin, int n) {
    FILE *fp = fopen(arquivo_bin, "rb");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* ler e verificar cabeçalho uma única vez */
    Head head;
    if (head_ler(fp, &head) != HEAD_OK || head.status == STATUS_INCONSISTENTE) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    /* buffers reutilizáveis para os critérios de cada busca */
    char campos[MAX_CRITERIOS][MAX_NOME_CAMPO];
    char valores[MAX_CRITERIOS][MAX_VALOR_CAMPO];

    /* --- executar cada uma das n buscas --- */
    for (int q = 0; q < n; q++) {
        int m; /* número de critérios desta busca */
        scanf("%d", &m);

        /* ler os m pares (campo, valor) */
        for (int i = 0; i < m; i++) {
            scanf("%s", campos[i]);
            /* ScanQuoteString lida com aspas, NULO e valores simples */
            ScanQuoteString(valores[i]);
        }

        /* busca sequencial: posicionar logo após o cabeçalho */
        fseek(fp, TAM_CABECALHO, SEEK_SET);

        int encontrou = 0;
        Registro r;
        RegistroStatus reg_status;

        while ((reg_status = registro_ler_bin(fp, &r)) == REG_OK) {
            /* ignorar registros removidos */
            if (r.removido == '1') {
                registro_free(&r);
                continue;
            }

            /* verificar se o registro satisfaz TODOS os critérios */
            int satisfaz = 1;
            for (int i = 0; i < m && satisfaz; i++) {
                if (!registro_match(&r, campos[i], valores[i])) {
                    satisfaz = 0;
                }
            }

            if (satisfaz) {
                registro_imprimir(&r);
                encontrou = 1;
            }

            registro_free(&r);
        }

        if (!encontrou) {
            printf("Registro inexistente.\n");
        }
    }

    fclose(fp);
}

/* -----------------------------------------------------------------------
 * Funcionalidade [4] - SELECT por RRN: acesso direto por posição
 * ----------------------------------------------------------------------- */

void func_select_rrn(const char *arquivo_bin, int rrn) {
    FILE *fp = fopen(arquivo_bin, "rb");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    /* ler e verificar cabeçalho */
    Head head;
    if (head_ler(fp, &head) != HEAD_OK || head.status == STATUS_INCONSISTENTE) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    /* validar RRN: deve estar no intervalo [0, proxRRN) */
    if (rrn < 0 || rrn >= head.proxRRN) {
        printf("Registro inexistente.\n");
        fclose(fp);
        return;
    }

    /* calcular offset e posicionar: offset = 17 + RRN * 80 */
    long offset = registro_rrn_offset(rrn);
    if (fseek(fp, offset, SEEK_SET) != 0) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    /* ler o registro */
    Registro r = registro_start();
    RegistroStatus reg_status = registro_ler_bin(fp, &r);

    if (reg_status != REG_OK || r.removido == '1') {
        printf("Registro inexistente.\n");
    } else {
        registro_imprimir(&r);
    }

    registro_free(&r);
    fclose(fp);
}
