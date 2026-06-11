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
#include "indice.h"
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
        /* linha em branco separando as consultas */
        if (q > 0) printf("\n");

        int m; /* número de critérios desta busca */
        scanf("%d", &m);

        /* ler os m pares (campo, valor) */
        for (int i = 0; i < m; i++) {
            scanf("%s", campos[i]);

            /* ScanQuoteString so funciona bem pra strings entre aspas e NULO.
               pra campos inteiros ela acaba consumindo o proximo token da entrada
               (por causa do getchar + scanf interno), entao usamos scanf direto */
            if (strcmp(campos[i], "nomeEstacao") == 0 || strcmp(campos[i], "nomeLinha") == 0) {
                ScanQuoteString(valores[i]);
            } else {
                scanf("%s", valores[i]);
                /* scanf le "NULO" literalmente, converter pra "" que eh
                   o que registro_match espera pra indicar campo nulo */
                if (strcmp(valores[i], "NULO") == 0)
                    valores[i][0] = '\0';
            }
        }

        /* codEstacao é o id (chave única): se for um dos critérios,
           podemos parar a busca assim que o registro for encontrado */
        int busca_por_id = 0;
        for (int i = 0; i < m; i++) {
            if (strcmp(campos[i], "codEstacao") == 0) {
                busca_por_id = 1;
                break;
            }
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

            /* busca por id: como codEstacao é único, para de percorrer
               o arquivo assim que o registro é encontrado */
            if (satisfaz && busca_por_id) {
                break;
            }
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

/* -----------------------------------------------------------------------
 * Auxiliares de entrada compartilhadas pelas funcionalidades [6]..[9]
 * ----------------------------------------------------------------------- */

/*
 * Lê um par (campo, valor) da entrada seguindo a regra do projeto:
 *  - nomeEstacao / nomeLinha  -> valor entre aspas duplas (ScanQuoteString)
 *  - demais campos (inteiros) -> token simples; "NULO" vira "" (campo nulo)
 */
static void ler_par_campo_valor(char *campo, char *valor) {
    scanf("%s", campo);
    if (strcmp(campo, "nomeEstacao") == 0 || strcmp(campo, "nomeLinha") == 0) {
        ScanQuoteString(valor);
    } else {
        scanf("%s", valor);
        if (strcmp(valor, "NULO") == 0) valor[0] = '\0';
    }
}

/* Lê um inteiro que pode vir como "NULO" (retorna -1 nesse caso). */
static int ler_inteiro_ou_nulo(void) {
    char tok[MAX_VALOR_CAMPO];
    scanf("%s", tok);
    if (strcmp(tok, "NULO") == 0) return -1;
    return atoi(tok);
}

/*
 * Verifica se o registro r (já lido e ativo) satisfaz TODOS os m critérios.
 * Retorna 1 se satisfaz, 0 caso contrário.
 */
static int satisfaz_criterios(const Registro *r, int m,
                              char campos[][MAX_NOME_CAMPO],
                              char valores[][MAX_VALOR_CAMPO]) {
    for (int i = 0; i < m; i++) {
        if (!registro_match(r, campos[i], valores[i])) return 0;
    }
    return 1;
}

/*
 * Procura, no critério de busca, o valor de codEstacao (chave do índice).
 * Retorna 1 e preenche *cod se codEstacao estiver entre os critérios;
 * retorna 0 caso contrário (a busca deverá ser sequencial).
 */
static int criterio_tem_codEstacao(int m, char campos[][MAX_NOME_CAMPO],
                                    char valores[][MAX_VALOR_CAMPO], int *cod) {
    for (int i = 0; i < m; i++) {
        if (strcmp(campos[i], "codEstacao") == 0 && valores[i][0] != '\0') {
            *cod = atoi(valores[i]);
            return 1;
        }
    }
    return 0;
}

/*
 * Coleta os RRNs dos registros ativos que satisfazem os m critérios.
 * Usa o índice (busca binária) quando codEstacao é um dos critérios;
 * caso contrário faz varredura sequencial no arquivo de dados.
 * Retorna a quantidade de RRNs coletados em rrns_out (vetor já alocado).
 */
static int coletar_rrns(FILE *fp, const Indice *idx,
                        int m, char campos[][MAX_NOME_CAMPO],
                        char valores[][MAX_VALOR_CAMPO], int *rrns_out) {
    int total = 0;
    int cod;

    if (criterio_tem_codEstacao(m, campos, valores, &cod)) {
        /* busca indexada: no máximo um registro (codEstacao é único) */
        int rrn = indice_buscar(idx, cod);
        if (rrn >= 0) {
            Registro r;
            fseek(fp, registro_rrn_offset(rrn), SEEK_SET);
            if (registro_ler_bin(fp, &r) == REG_OK && r.removido == '0' &&
                satisfaz_criterios(&r, m, campos, valores)) {
                rrns_out[total++] = rrn;
            }
            registro_free(&r);
        }
    } else {
        /* busca sequencial */
        fseek(fp, TAM_CABECALHO, SEEK_SET);
        Registro r;
        int rrn = 0;
        while (registro_ler_bin(fp, &r) == REG_OK) {
            if (r.removido == '0' && satisfaz_criterios(&r, m, campos, valores)) {
                rrns_out[total++] = rrn;
            }
            registro_free(&r);
            rrn++;
        }
    }
    return total;
}

/*
 * Reconta o número de estações distintas (por nomeEstacao) entre os
 * registros ativos do arquivo. Usado para manter nroEstacoes coerente
 * após remoções/inserções/atualizações.
 */
static int recontar_nro_estacoes(FILE *fp) {
    ListaNomes nomes = lista_nomes_criar();
    fseek(fp, TAM_CABECALHO, SEEK_SET);
    Registro r;
    while (registro_ler_bin(fp, &r) == REG_OK) {
        if (r.removido == '0' && r.nomeEstacao != NULL &&
            !lista_nomes_contem(&nomes, r.nomeEstacao)) {
            lista_nomes_inserir(&nomes, r.nomeEstacao);
        }
        registro_free(&r);
    }
    int total = nomes.count;
    lista_nomes_destruir(&nomes);
    return total;
}

/* -----------------------------------------------------------------------
 * Funcionalidade [5] - Criar arquivo de índice primário a partir dos dados
 * ----------------------------------------------------------------------- */

void func_criar_indice(const char *arquivo_bin, const char *arquivo_indice) {
    FILE *fp = fopen(arquivo_bin, "rb");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Head head;
    if (head_ler(fp, &head) != HEAD_OK || head.status == STATUS_INCONSISTENTE) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    Indice idx;
    indice_start(&idx);

    /* percorrer os registros sequencialmente; o RRN é a ordem de leitura */
    fseek(fp, TAM_CABECALHO, SEEK_SET);
    Registro r;
    RegistroStatus st;
    int rrn = 0;
    while ((st = registro_ler_bin(fp, &r)) == REG_OK) {
        if (r.removido == '0') {
            /* indice_inserir mantém a ordem crescente por codEstacao */
            indice_inserir(&idx, r.codEstacao, rrn);
        }
        registro_free(&r);
        rrn++;
    }
    fclose(fp);

    if (st == REG_ERRO) {
        printf("Falha no processamento do arquivo.\n");
        indice_free(&idx);
        return;
    }

    if (indice_salvar(&idx, arquivo_indice) != INDICE_OK) {
        printf("Falha no processamento do arquivo.\n");
        indice_free(&idx);
        return;
    }
    indice_free(&idx);

    /* exibir o índice gerado (obrigatório conforme especificação) */
    BinarioNaTela((char *)arquivo_indice);
}

/* -----------------------------------------------------------------------
 * Funcionalidade [6] - SELECT WHERE com índice (busca indexada/sequencial)
 * ----------------------------------------------------------------------- */

void func_select_where_indexado(const char *arquivo_bin,
                                const char *arquivo_indice, int n) {
    FILE *fp = fopen(arquivo_bin, "rb");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Head head;
    if (head_ler(fp, &head) != HEAD_OK || head.status == STATUS_INCONSISTENTE) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    Indice idx;
    indice_start(&idx);
    if (indice_carregar(&idx, arquivo_indice) != INDICE_OK) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    char campos[MAX_CRITERIOS][MAX_NOME_CAMPO];
    char valores[MAX_CRITERIOS][MAX_VALOR_CAMPO];

    for (int q = 0; q < n; q++) {
        if (q > 0) printf("\n");

        int m;
        scanf("%d", &m);
        for (int i = 0; i < m; i++) ler_par_campo_valor(campos[i], valores[i]);

        int cod;
        int encontrou = 0;

        if (criterio_tem_codEstacao(m, campos, valores, &cod)) {
            /* busca indexada via busca binária no índice em RAM */
            int rrn = indice_buscar(&idx, cod);
            if (rrn >= 0) {
                Registro r;
                fseek(fp, registro_rrn_offset(rrn), SEEK_SET);
                if (registro_ler_bin(fp, &r) == REG_OK && r.removido == '0' &&
                    satisfaz_criterios(&r, m, campos, valores)) {
                    registro_imprimir(&r);
                    encontrou = 1;
                }
                registro_free(&r);
            }
        } else {
            /* busca sequencial no arquivo de dados */
            fseek(fp, TAM_CABECALHO, SEEK_SET);
            Registro r;
            while (registro_ler_bin(fp, &r) == REG_OK) {
                if (r.removido == '0' &&
                    satisfaz_criterios(&r, m, campos, valores)) {
                    registro_imprimir(&r);
                    encontrou = 1;
                }
                registro_free(&r);
            }
        }

        if (!encontrou) printf("Registro inexistente.\n");
    }

    indice_free(&idx);
    fclose(fp);
}

/* -----------------------------------------------------------------------
 * Funcionalidade [7] - DELETE: remoção lógica + atualização do índice
 * ----------------------------------------------------------------------- */

void func_remover(const char *arquivo_bin, const char *arquivo_indice, int n) {
    FILE *fp = fopen(arquivo_bin, "r+b");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Head head;
    if (head_ler(fp, &head) != HEAD_OK || head.status == STATUS_INCONSISTENTE) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    Indice idx;
    indice_start(&idx);
    if (indice_carregar(&idx, arquivo_indice) != INDICE_OK) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    /* marcar arquivo como inconsistente durante a escrita */
    head.status = STATUS_INCONSISTENTE;
    head_escrever(fp, &head);

    char campos[MAX_CRITERIOS][MAX_NOME_CAMPO];
    char valores[MAX_CRITERIOS][MAX_VALOR_CAMPO];
    int *rrns = (int *)malloc((head.proxRRN > 0 ? head.proxRRN : 1) * sizeof(int));
    int total_removidos = 0;

    for (int q = 0; q < n; q++) {
        int m;
        scanf("%d", &m);
        for (int i = 0; i < m; i++) ler_par_campo_valor(campos[i], valores[i]);

        int qtd = coletar_rrns(fp, &idx, m, campos, valores, rrns);
        total_removidos += qtd;

        /* remover cada registro coletado */
        for (int k = 0; k < qtd; k++) {
            int rrn = rrns[k];

            /* ler o registro para obter o codEstacao (remoção no índice) */
            Registro r;
            fseek(fp, registro_rrn_offset(rrn), SEEK_SET);
            if (registro_ler_bin(fp, &r) != REG_OK) { registro_free(&r); continue; }
            int cod = r.codEstacao;
            registro_free(&r);

            /* remoção lógica: sobrescreve apenas removido(1 byte)='1' e
               proximo(4 bytes), encadeando o RRN na pilha de removidos */
            char removido = '1';
            int  proximo  = head.topo;
            fseek(fp, registro_rrn_offset(rrn), SEEK_SET);
            fwrite(&removido, 1, 1, fp);
            fwrite(&proximo,  4, 1, fp);
            head.topo = rrn;

            /* remover a entrada do índice primário */
            indice_remover(&idx, cod);
        }
    }

    free(rrns);

    /* atualizar contadores: nroEstacoes recontado (estações distintas ativas);
       nroParesEstacao reduzido em 1 por registro removido */
    head.nroEstacoes      = recontar_nro_estacoes(fp);
    head.nroParesEstacao -= total_removidos;

    /* finalizar: cabeçalho consistente e persistir índice */
    head.status = STATUS_CONSISTENTE;
    head_escrever(fp, &head);
    fclose(fp);

    BinarioNaTela((char *)arquivo_bin);

    if (indice_salvar(&idx, arquivo_indice) != INDICE_OK) {
        printf("Falha no processamento do arquivo.\n");
        indice_free(&idx);
        return;
    }
    indice_free(&idx);

    BinarioNaTela((char *)arquivo_indice);
}

/* -----------------------------------------------------------------------
 * Funcionalidade [8] - INSERT: inserção com reaproveitamento da pilha
 * ----------------------------------------------------------------------- */

void func_inserir(const char *arquivo_bin, const char *arquivo_indice, int n) {
    FILE *fp = fopen(arquivo_bin, "r+b");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Head head;
    if (head_ler(fp, &head) != HEAD_OK || head.status == STATUS_INCONSISTENTE) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    Indice idx;
    indice_start(&idx);
    if (indice_carregar(&idx, arquivo_indice) != INDICE_OK) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    head.status = STATUS_INCONSISTENTE;
    head_escrever(fp, &head);

    char buffer[MAX_VALOR_CAMPO];

    for (int q = 0; q < n; q++) {
        /* ler os 8 campos posicionais do novo registro */
        Registro novo = registro_start();
        novo.removido = '0';
        novo.proximo  = -1;

        novo.codEstacao = ler_inteiro_ou_nulo();          /* codEstacao */

        ScanQuoteString(buffer);                           /* nomeEstacao */
        registro_set_str(&novo, REG_CAMPO_NOME_ESTACAO, buffer);

        novo.codLinha = ler_inteiro_ou_nulo();             /* codLinha */

        ScanQuoteString(buffer);                           /* nomeLinha */
        registro_set_str(&novo, REG_CAMPO_NOME_LINHA, buffer);

        novo.codProxEstacao  = ler_inteiro_ou_nulo();      /* codProxEstacao */
        novo.distProxEstacao = ler_inteiro_ou_nulo();      /* distProxEstacao */
        novo.codLinhaIntegra = ler_inteiro_ou_nulo();      /* codLinhaIntegra */
        novo.codEstIntegra   = ler_inteiro_ou_nulo();      /* codEstIntegra */

        /* escolher o RRN: reaproveitar da pilha de removidos ou inserir no fim */
        int rrn;
        if (head.topo != -1) {
            rrn = head.topo;
            /* recuperar o 'proximo' do registro removido para desempilhar */
            Registro rem;
            fseek(fp, registro_rrn_offset(rrn), SEEK_SET);
            registro_ler_bin(fp, &rem);
            head.topo = rem.proximo;
            registro_free(&rem);
        } else {
            rrn = head.proxRRN;
            head.proxRRN++;
        }

        /* gravar o novo registro na posição escolhida */
        fseek(fp, registro_rrn_offset(rrn), SEEK_SET);
        registro_escrever_bin(fp, &novo);

        /* inserir no índice primário mantendo a ordem por codEstacao */
        indice_inserir(&idx, novo.codEstacao, rrn);

        registro_free(&novo);
    }

    /* atualizar contadores: nroEstacoes recontado; nroParesEstacao
       acrescido em 1 por registro inserido (simétrico à remoção) */
    head.nroEstacoes      = recontar_nro_estacoes(fp);
    head.nroParesEstacao += n;

    head.status = STATUS_CONSISTENTE;
    head_escrever(fp, &head);
    fclose(fp);

    BinarioNaTela((char *)arquivo_bin);

    if (indice_salvar(&idx, arquivo_indice) != INDICE_OK) {
        printf("Falha no processamento do arquivo.\n");
        indice_free(&idx);
        return;
    }
    indice_free(&idx);

    BinarioNaTela((char *)arquivo_indice);
}

/* -----------------------------------------------------------------------
 * Funcionalidade [9] - UPDATE: atualização in-place + manutenção do índice
 * ----------------------------------------------------------------------- */

void func_atualizar(const char *arquivo_bin, const char *arquivo_indice, int n) {
    FILE *fp = fopen(arquivo_bin, "r+b");
    if (fp == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    Head head;
    if (head_ler(fp, &head) != HEAD_OK || head.status == STATUS_INCONSISTENTE) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    Indice idx;
    indice_start(&idx);
    if (indice_carregar(&idx, arquivo_indice) != INDICE_OK) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fp);
        return;
    }

    head.status = STATUS_INCONSISTENTE;
    head_escrever(fp, &head);

    /* critérios de busca (WHERE) */
    char campos[MAX_CRITERIOS][MAX_NOME_CAMPO];
    char valores[MAX_CRITERIOS][MAX_VALOR_CAMPO];
    /* campos a atualizar (SET) */
    char setCampos[MAX_CRITERIOS][MAX_NOME_CAMPO];
    char setValores[MAX_CRITERIOS][MAX_VALOR_CAMPO];

    int *rrns = (int *)malloc((head.proxRRN > 0 ? head.proxRRN : 1) * sizeof(int));

    for (int q = 0; q < n; q++) {
        /* ler critério de busca: mB pares (campo, valor) */
        int mB;
        scanf("%d", &mB);
        for (int i = 0; i < mB; i++) ler_par_campo_valor(campos[i], valores[i]);

        /* ler atualização: mA pares (campo, valor) */
        int mA;
        scanf("%d", &mA);
        for (int i = 0; i < mA; i++) ler_par_campo_valor(setCampos[i], setValores[i]);

        int qtd = coletar_rrns(fp, &idx, mB, campos, valores, rrns);

        for (int k = 0; k < qtd; k++) {
            int rrn = rrns[k];

            /* ler o registro a atualizar */
            Registro r;
            fseek(fp, registro_rrn_offset(rrn), SEEK_SET);
            if (registro_ler_bin(fp, &r) != REG_OK) { registro_free(&r); continue; }

            int cod_antigo = r.codEstacao;

            /* aplicar cada campo do SET */
            for (int i = 0; i < mA; i++) {
                registro_set_por_nome(&r, setCampos[i], setValores[i]);
            }

            /* regravar in-place (registro continua com 80 bytes fixos) */
            fseek(fp, registro_rrn_offset(rrn), SEEK_SET);
            registro_escrever_bin(fp, &r);

            /* se o codEstacao mudou, atualizar a chave no índice (mesmo RRN) */
            if (r.codEstacao != cod_antigo) {
                indice_remover(&idx, cod_antigo);
                indice_inserir(&idx, r.codEstacao, rrn);
            }

            registro_free(&r);
        }
    }

    free(rrns);

    /* atualizar nroEstacoes (atualizações podem alterar nomeEstacao);
       o número de registros não muda, então nroParesEstacao é mantido */
    head.nroEstacoes = recontar_nro_estacoes(fp);

    head.status = STATUS_CONSISTENTE;
    head_escrever(fp, &head);
    fclose(fp);

    BinarioNaTela((char *)arquivo_bin);

    if (indice_salvar(&idx, arquivo_indice) != INDICE_OK) {
        printf("Falha no processamento do arquivo.\n");
        indice_free(&idx);
        return;
    }
    indice_free(&idx);

    BinarioNaTela((char *)arquivo_indice);
}
