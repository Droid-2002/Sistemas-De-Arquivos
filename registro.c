/*
 * registro.c - Implementação do TAD Registro.
 *
 * Funções de inicialização, leitura, escrita, impressão e busca
 * de registros de dados de 80 bytes no arquivo binário.
 *
 * IMPORTANTE: toda escrita e leitura é feita campo a campo com fwrite/fread
 * individuais, nunca como struct inteira (evita problemas de padding).
 */

#include "registro.h"
#include "head.h"       /* para TAM_CABECALHO */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Tamanhos em bytes de cada campo do registro */
#define TAM_REGISTRO              80
#define TAM_REMOVIDO               1
#define TAM_PROXIMO                4
#define TAM_COD_ESTACAO            4
#define TAM_COD_LINHA              4
#define TAM_COD_PROX_ESTACAO       4
#define TAM_DIST_PROX_ESTACAO      4
#define TAM_COD_LINHA_INTEGRA      4
#define TAM_COD_EST_INTEGRA        4
#define TAM_INDICE_NOME_ESTACAO    4
#define TAM_INDICE_NOME_LINHA      4

/* Bytes fixos totais por registro (37 = soma dos campos fixos + indicadores) */
#define BYTES_FIXOS_REGISTRO      37

#define BYTE_LIXO '$'

/* -----------------------------------------------------------------------
 * Inicialização e liberação
 * ----------------------------------------------------------------------- */

Registro registro_start(void) {
    Registro r;
    r.removido        = '0';
    r.proximo         = -1;
    r.codEstacao      = -1;
    r.codLinha        = -1;
    r.codProxEstacao  = -1;
    r.distProxEstacao = -1;
    r.codLinhaIntegra = -1;
    r.codEstIntegra   = -1;
    r.tamNomeEstacao  = 0;
    r.nomeEstacao     = NULL;
    r.tamNomeLinha    = 0;
    r.nomeLinha       = NULL;
    return r;
}

void registro_free(Registro *r) {
    if (r == NULL) return;

    /* libera nomeEstacao se alocado */
    if (r->nomeEstacao != NULL) {
        free(r->nomeEstacao);
        r->nomeEstacao    = NULL;
        r->tamNomeEstacao = 0;
    }

    /* libera nomeLinha se alocado */
    if (r->nomeLinha != NULL) {
        free(r->nomeLinha);
        r->nomeLinha    = NULL;
        r->tamNomeLinha = 0;
    }
}

/* -----------------------------------------------------------------------
 * Setters seguros para strings
 * ----------------------------------------------------------------------- */

/*
 * Define o nome da estação, alocando cópia interna.
 * nome=NULL ou tam_nome=0 limpa o campo.
 */
RegistroStatus registro_set_nome_estacao(Registro *r, const char *nome, int tam_nome) {
    if (r == NULL) return REG_ERRO;

    /* validações básicas */
    if (nome != NULL && tam_nome == 0) return REG_ERRO;
    if (nome != NULL && (int)strlen(nome) != tam_nome) return REG_ERRO;

    /* liberar memória anterior se existir */
    if (r->nomeEstacao != NULL) {
        free(r->nomeEstacao);
        r->nomeEstacao    = NULL;
        r->tamNomeEstacao = 0;
    }

    if (nome == NULL) return REG_OK; /* manter como nulo */

    /* alocar e copiar novo nome (com '\0' em memória) */
    r->nomeEstacao = (char *)malloc(tam_nome + 1);
    if (r->nomeEstacao == NULL) return REG_ERRO;

    strcpy(r->nomeEstacao, nome);
    r->tamNomeEstacao = tam_nome;
    return REG_OK;
}

/* Define o nome da linha, alocando cópia interna. NULL/0 = campo nulo. */
RegistroStatus registro_set_nome_linha(Registro *r, const char *nome, int tam_nome) {
    if (r == NULL) return REG_ERRO;

    if (nome != NULL && tam_nome == 0) return REG_ERRO;
    if (nome != NULL && (int)strlen(nome) != tam_nome) return REG_ERRO;

    if (r->nomeLinha != NULL) {
        free(r->nomeLinha);
        r->nomeLinha    = NULL;
        r->tamNomeLinha = 0;
    }

    if (nome == NULL) return REG_OK;

    r->nomeLinha = (char *)malloc(tam_nome + 1);
    if (r->nomeLinha == NULL) return REG_ERRO;

    strcpy(r->nomeLinha, nome);
    r->tamNomeLinha = tam_nome;
    return REG_OK;
}

/* -----------------------------------------------------------------------
 * Setters genéricos (por campo)
 * ----------------------------------------------------------------------- */

RegistroStatus registro_set_char(Registro *r, RegistroCampo campo, char valor) {
    if (r == NULL) return REG_ERRO;
    if (campo == REG_CAMPO_REMOVIDO) {
        r->removido = valor;
        return REG_OK;
    }
    return REG_ERRO;
}

RegistroStatus registro_set_int(Registro *r, RegistroCampo campo, int valor) {
    if (r == NULL) return REG_ERRO;
    switch (campo) {
        case REG_CAMPO_PROXIMO:             r->proximo         = valor; return REG_OK;
        case REG_CAMPO_COD_ESTACAO:         r->codEstacao      = valor; return REG_OK;
        case REG_CAMPO_COD_LINHA:           r->codLinha        = valor; return REG_OK;
        case REG_CAMPO_COD_PROX_ESTACAO:    r->codProxEstacao  = valor; return REG_OK;
        case REG_CAMPO_DIST_PROX_ESTACAO:   r->distProxEstacao = valor; return REG_OK;
        case REG_CAMPO_COD_LINHA_INTEGRA:   r->codLinhaIntegra = valor; return REG_OK;
        case REG_CAMPO_COD_EST_INTEGRA:     r->codEstIntegra   = valor; return REG_OK;
        default: return REG_ERRO;
    }
}

RegistroStatus registro_set_str(Registro *r, RegistroCampo campo, const char *valor) {
    if (r == NULL) return REG_ERRO;

    /* valor NULL ou vazio significa campo nulo */
    int tam = (valor == NULL || valor[0] == '\0') ? 0 : (int)strlen(valor);

    if (campo == REG_CAMPO_NOME_ESTACAO) {
        return registro_set_nome_estacao(r, (tam > 0 ? valor : NULL), tam);
    }
    if (campo == REG_CAMPO_NOME_LINHA) {
        return registro_set_nome_linha(r, (tam > 0 ? valor : NULL), tam);
    }
    return REG_ERRO;
}

/* -----------------------------------------------------------------------
 * Cálculos auxiliares
 * ----------------------------------------------------------------------- */

/* Retorna quantos bytes de '$' são necessários para completar 80 bytes */
int registro_bytes_lixo(const Registro *r) {
    /* bytes fixos = 37 (ver BYTES_FIXOS_REGISTRO) + strings variáveis */
    return TAM_REGISTRO - BYTES_FIXOS_REGISTRO - r->tamNomeEstacao - r->tamNomeLinha;
}

/* Offset em bytes de um registro com um dado RRN */
long registro_rrn_offset(int rrn) {
    return (long)TAM_CABECALHO + (long)rrn * TAM_REGISTRO;
}

/* -----------------------------------------------------------------------
 * Escrita no arquivo binário
 * ----------------------------------------------------------------------- */

/*
 * Grava todos os campos do registro no arquivo, na ordem exata da especificação.
 * Após os campos variáveis, preenche o espaço restante com '$'.
 */
RegistroStatus registro_escrever_bin(FILE *fp, const Registro *r) {
    if (fp == NULL || r == NULL) return REG_ERRO;

    /* campos de controle */
    if (fwrite(&r->removido,         TAM_REMOVIDO,             1, fp) != 1) return REG_ERRO;
    if (fwrite(&r->proximo,          TAM_PROXIMO,              1, fp) != 1) return REG_ERRO;

    /* campos de dados fixos */
    if (fwrite(&r->codEstacao,       TAM_COD_ESTACAO,          1, fp) != 1) return REG_ERRO;
    if (fwrite(&r->codLinha,         TAM_COD_LINHA,            1, fp) != 1) return REG_ERRO;
    if (fwrite(&r->codProxEstacao,   TAM_COD_PROX_ESTACAO,     1, fp) != 1) return REG_ERRO;
    if (fwrite(&r->distProxEstacao,  TAM_DIST_PROX_ESTACAO,    1, fp) != 1) return REG_ERRO;
    if (fwrite(&r->codLinhaIntegra,  TAM_COD_LINHA_INTEGRA,    1, fp) != 1) return REG_ERRO;
    if (fwrite(&r->codEstIntegra,    TAM_COD_EST_INTEGRA,      1, fp) != 1) return REG_ERRO;

    /* indicador + conteúdo do nomeEstacao */
    if (fwrite(&r->tamNomeEstacao,   TAM_INDICE_NOME_ESTACAO,  1, fp) != 1) return REG_ERRO;
    if (r->tamNomeEstacao > 0) {
        /* string SEM '\0' no arquivo */
        if (fwrite(r->nomeEstacao, sizeof(char), r->tamNomeEstacao, fp)
                != (size_t)r->tamNomeEstacao) return REG_ERRO;
    }

    /* indicador + conteúdo do nomeLinha */
    if (fwrite(&r->tamNomeLinha,     TAM_INDICE_NOME_LINHA,    1, fp) != 1) return REG_ERRO;
    if (r->tamNomeLinha > 0) {
        if (fwrite(r->nomeLinha, sizeof(char), r->tamNomeLinha, fp)
                != (size_t)r->tamNomeLinha) return REG_ERRO;
    }

    /* preencher bytes restantes com '$' para completar 80 bytes */
    int bytes_lixo = registro_bytes_lixo(r);
    for (int i = 0; i < bytes_lixo; i++) {
        char lixo = BYTE_LIXO;
        if (fwrite(&lixo, sizeof(char), 1, fp) != 1) return REG_ERRO;
    }

    return REG_OK;
}

/* -----------------------------------------------------------------------
 * Leitura do arquivo binário
 * ----------------------------------------------------------------------- */

/*
 * Lê um registro do arquivo, campo a campo.
 *
 * Para registros removidos ('1'): lê o campo proximo para possível uso
 * no encadeamento da lista, pula os demais bytes e retorna REG_OK com
 * r->removido == '1'.
 *
 * Para registros ativos: lê todos os campos e aloca memória para strings.
 *
 * Retorna REG_EOF ao atingir o fim do arquivo antes de ler o primeiro byte.
 */
RegistroStatus registro_ler_bin(FILE *fp, Registro *r) {
    if (fp == NULL || r == NULL) return REG_ERRO;

    /* garantir que ponteiros estão limpos (evitar double-free) */
    r->nomeEstacao = NULL;
    r->nomeLinha   = NULL;

    /* ler o primeiro byte (removido): distinguir EOF de erro */
    if (fread(&r->removido, TAM_REMOVIDO, 1, fp) != 1) {
        if (feof(fp)) return REG_EOF;
        return REG_ERRO;
    }

    if (r->removido == '1') {
        /* registro removido: ler proximo para manter o encadeamento,
         * depois pular os 75 bytes restantes sem alocação de memória */
        if (fread(&r->proximo, TAM_PROXIMO, 1, fp) != 1) return REG_ERRO;
        /* 80 - 1(removido) - 4(proximo) = 75 bytes a pular */
        if (fseek(fp, 75, SEEK_CUR) != 0) return REG_ERRO;
        return REG_OK;
    }

    /* registro ativo: leitura completa de todos os campos */
    if (fread(&r->proximo,         TAM_PROXIMO,           1, fp) != 1) return REG_ERRO;
    if (fread(&r->codEstacao,      TAM_COD_ESTACAO,       1, fp) != 1) return REG_ERRO;
    if (fread(&r->codLinha,        TAM_COD_LINHA,         1, fp) != 1) return REG_ERRO;
    if (fread(&r->codProxEstacao,  TAM_COD_PROX_ESTACAO,  1, fp) != 1) return REG_ERRO;
    if (fread(&r->distProxEstacao, TAM_DIST_PROX_ESTACAO, 1, fp) != 1) return REG_ERRO;
    if (fread(&r->codLinhaIntegra, TAM_COD_LINHA_INTEGRA, 1, fp) != 1) return REG_ERRO;
    if (fread(&r->codEstIntegra,   TAM_COD_EST_INTEGRA,   1, fp) != 1) return REG_ERRO;

    /* nomeEstacao: ler tamanho, depois alocar e ler string */
    if (fread(&r->tamNomeEstacao,  TAM_INDICE_NOME_ESTACAO, 1, fp) != 1) return REG_ERRO;
    if (r->tamNomeEstacao > 0) {
        r->nomeEstacao = (char *)malloc(r->tamNomeEstacao + 1);
        if (r->nomeEstacao == NULL) return REG_ERRO;

        if (fread(r->nomeEstacao, sizeof(char), r->tamNomeEstacao, fp)
                != (size_t)r->tamNomeEstacao) {
            free(r->nomeEstacao);
            r->nomeEstacao = NULL;
            return REG_ERRO;
        }
        /* adicionar '\0' para uso em memória */
        r->nomeEstacao[r->tamNomeEstacao] = '\0';
    }

    /* nomeLinha: ler tamanho, depois alocar e ler string (pode ser 0 = nulo) */
    if (fread(&r->tamNomeLinha, TAM_INDICE_NOME_LINHA, 1, fp) != 1) return REG_ERRO;
    if (r->tamNomeLinha > 0) {
        r->nomeLinha = (char *)malloc(r->tamNomeLinha + 1);
        if (r->nomeLinha == NULL) {
            free(r->nomeEstacao);
            r->nomeEstacao = NULL;
            return REG_ERRO;
        }
        if (fread(r->nomeLinha, sizeof(char), r->tamNomeLinha, fp)
                != (size_t)r->tamNomeLinha) {
            free(r->nomeLinha);
            r->nomeLinha = NULL;
            free(r->nomeEstacao);
            r->nomeEstacao = NULL;
            return REG_ERRO;
        }
        r->nomeLinha[r->tamNomeLinha] = '\0';
    } else {
        r->nomeLinha = NULL;
    }

    /* pular bytes de lixo (não precisamos ler, apenas avançar o ponteiro) */
    int bytes_lixo = registro_bytes_lixo(r);
    if (bytes_lixo > 0) {
        if (fseek(fp, bytes_lixo, SEEK_CUR) != 0) return REG_ERRO;
    }

    return REG_OK;
}

/* -----------------------------------------------------------------------
 * Impressão no formato exigido
 * ----------------------------------------------------------------------- */

/*
 * Exibe o registro em uma linha, campos separados por espaço:
 * codEstacao nomeEstacao codLinha nomeLinha codProxEstacao distProxEstacao codLinhaIntegra codEstIntegra
 * Valores nulos inteiros (-1) → "NULO". Strings nulas → "NULO".
 */
void registro_imprimir(const Registro *r) {
    if (r == NULL) return;

    /* codEstacao e nomeEstacao nunca são nulos */
    printf("%d", r->codEstacao);
    printf(" %s", r->nomeEstacao);

    /* codLinha */
    if (r->codLinha == -1) printf(" NULO");
    else                   printf(" %d", r->codLinha);

    /* nomeLinha */
    if (r->nomeLinha == NULL) printf(" NULO");
    else                      printf(" %s", r->nomeLinha);

    /* codProxEstacao */
    if (r->codProxEstacao == -1) printf(" NULO");
    else                         printf(" %d", r->codProxEstacao);

    /* distProxEstacao */
    if (r->distProxEstacao == -1) printf(" NULO");
    else                          printf(" %d", r->distProxEstacao);

    /* codLinhaIntegra */
    if (r->codLinhaIntegra == -1) printf(" NULO");
    else                          printf(" %d", r->codLinhaIntegra);

    /* codEstIntegra */
    if (r->codEstIntegra == -1) printf(" NULO");
    else                        printf(" %d", r->codEstIntegra);

    printf("\n");
}

/* -----------------------------------------------------------------------
 * Match para busca por campos (funcionalidade [3])
 * ----------------------------------------------------------------------- */

/*
 * Verifica se o registro satisfaz um critério de busca.
 * campo: nome do campo (ex: "codLinha", "nomeEstacao")
 * valor: valor esperado; "" indica busca por NULO
 *        (ScanQuoteString retorna "" para a palavra NULO na entrada)
 * Retorna 1 se satisfaz, 0 caso contrário.
 */
int registro_match(const Registro *r, const char *campo, const char *valor) {
    if (r == NULL || campo == NULL || valor == NULL) return 0;

    /* indica busca por NULO: string vazia retornada por ScanQuoteString */
    int busca_nulo = (valor[0] == '\0');

    /* --- campos inteiros --- */
    if (strcmp(campo, "codEstacao") == 0) {
        /* codEstacao nunca é NULO */
        if (busca_nulo) return 0;
        return r->codEstacao == atoi(valor);
    }
    if (strcmp(campo, "codLinha") == 0) {
        if (busca_nulo) return r->codLinha == -1;
        return r->codLinha == atoi(valor);
    }
    if (strcmp(campo, "codProxEstacao") == 0) {
        if (busca_nulo) return r->codProxEstacao == -1;
        return r->codProxEstacao == atoi(valor);
    }
    if (strcmp(campo, "distProxEstacao") == 0) {
        if (busca_nulo) return r->distProxEstacao == -1;
        return r->distProxEstacao == atoi(valor);
    }
    if (strcmp(campo, "codLinhaIntegra") == 0) {
        if (busca_nulo) return r->codLinhaIntegra == -1;
        return r->codLinhaIntegra == atoi(valor);
    }
    if (strcmp(campo, "codEstIntegra") == 0) {
        if (busca_nulo) return r->codEstIntegra == -1;
        return r->codEstIntegra == atoi(valor);
    }

    /* --- campos string --- */
    if (strcmp(campo, "nomeEstacao") == 0) {
        if (busca_nulo) return (r->nomeEstacao == NULL || r->tamNomeEstacao == 0);
        if (r->nomeEstacao == NULL) return 0;
        return strcmp(r->nomeEstacao, valor) == 0;
    }
    if (strcmp(campo, "nomeLinha") == 0) {
        if (busca_nulo) return (r->nomeLinha == NULL || r->tamNomeLinha == 0);
        if (r->nomeLinha == NULL) return 0;
        return strcmp(r->nomeLinha, valor) == 0;
    }

    return 0; /* campo não reconhecido */
}
