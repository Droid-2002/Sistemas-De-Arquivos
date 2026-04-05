#include "../headers/registro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#define BYTE_LIXO '$'


Registro registro_start(void) {
    Registro r;
    r.removido = '0';
    r.proximo = -1;
    r.codEstacao = -1;
    r.codLinha = -1;
    r.codProxEstacao = -1;
    r.distProxEstacao = -1;
    r.codLinhaIntegra = -1;
    r.codEstIntegra = -1;
    r.tamNomeEstacao = 0;
    r.nomeEstacao = NULL;
    r.tamNomeLinha = 0;
    r.nomeLinha = NULL;

    return r;
}

void registro_free(Registro *r) {
    if (r == NULL) { // Verificar ponteiro nulo
        return;
    }
    if (r->nomeEstacao != NULL) { // Liberar memória alocada para nomeEstacao
        free(r->nomeEstacao);
        r->nomeEstacao = NULL; // Depois de liberar ponteiro, marcar como NULL (poda de ponteiro pendente)
        // mudar tamanho do nome para 0
        r->tamNomeEstacao = 0;
    } else {
        r->nomeEstacao = NULL;
        r->tamNomeEstacao = 0;
    }
    if (r->nomeLinha != NULL) { // Liberar memória alocada para nomeLinha
        free(r->nomeLinha);
        r->nomeLinha = NULL; // Depois de liberar ponteiro, marcar como NULL (poda de ponteiro pendente)
        r->tamNomeLinha = 0;
    } else {
        r->nomeLinha = NULL;
        r->tamNomeLinha = 0;
    }
}

/*
        Para setar o nome da estação no regsitro em memória, precisamos:
        1. verificar se já existe um nome definido, pois se tiver precisa 
        liberar a memória alocada para o nome antigo antes de alocar para o novo nome
        e arrumar os ponteiros para evitar vazamento de memória.
        2. alocar memória para o novo nome, considerando o tamanho da string + 1 para o caractere nulo
        3. copiar o conteúdo do nome fornecido para a memória alocada
    */
RegistroStatus registro_set_nome_estacao(Registro *r, const char *nome, int tam_nome) {
    // Verificar ponteiros nulos
    if (r == NULL) {
        return REG_ERRO;
    }
    if (nome != NULL  && tam_nome == 0) {
        return REG_ERRO;
    } else if (nome != NULL && strlen(nome) != tam_nome) {
        return REG_ERRO;
    }

    // se novo nome == nulo e nome antigo == nulo, mantém estado atual
    if (nome == NULL && r->nomeEstacao == NULL) {
        return REG_OK;

    } else if (nome == NULL && r->nomeEstacao != NULL) { // se novo nome == nulo e antigo for diferente, libera memoria e seta Nulo
        free(r->nomeEstacao);
        r->nomeEstacao = NULL;
        r->tamNomeEstacao = 0;
        return REG_OK;

    } else if (nome != NULL && r->nomeEstacao == NULL) { // se novo nome != nulo e antigo == nulo
        r->nomeEstacao = (char *)malloc(tam_nome + 1);
        if (r->nomeEstacao == NULL) { // Verificar se a alocação foi bem sucedida
            return REG_ERRO;
        }
        strcpy(r->nomeEstacao, nome);
        r->tamNomeEstacao = tam_nome;
        return REG_OK;

    } else { // se novo nome e antigo != de nulo
        char *temp = (char *)malloc(tam_nome + 1);
        if (temp == NULL) { // Verificar se a alocação foi bem sucedida
            return REG_ERRO;
        }
        strcpy(temp, nome);
        free(r->nomeEstacao);
        r->nomeEstacao = temp;
        r->tamNomeEstacao = tam_nome;
        return REG_OK;
    }
}


RegistroStatus registro_set_nome_linha(Registro *r, const char *nome, int tam_nome) {
    // Verificar ponteiros nulos
    if (r == NULL) {
        return REG_ERRO;
    }
    if (nome != NULL  && tam_nome == 0) {
        return REG_ERRO;
    } else if (nome != NULL && strlen(nome) != tam_nome) {
        return REG_ERRO;
    }

    // se novo nome == nulo e nome antigo == nulo, mantém estado atual
    if (nome == NULL && r->nomeLinha == NULL) {
        return REG_OK;

    } else if (nome == NULL && r->nomeLinha != NULL) { // se novo nome == nulo e antigo for diferente, libera memoria e seta Nulo
        free(r->nomeLinha);
        r->nomeLinha = NULL;
        r->tamNomeLinha = 0;
        return REG_OK;

    } else if (nome != NULL && r->nomeLinha == NULL) { // se novo nome != nulo e antigo == nulo
        r->nomeLinha = (char *)malloc(tam_nome + 1);
        if (r->nomeLinha == NULL) { // Verificar se a alocação foi bem sucedida
            return REG_ERRO;
        }
        strcpy(r->nomeLinha, nome);
        r->tamNomeLinha = tam_nome;
        return REG_OK;

    } else { // se novo nome e antigo != de nulo
        char *temp = (char *)malloc(tam_nome + 1);
        if (temp == NULL) { // Verificar se a alocação foi bem sucedida
            return REG_ERRO;
        }
        strcpy(temp, nome);
        free(r->nomeLinha);
        r->nomeLinha = temp;
        r->tamNomeLinha = tam_nome;
        return REG_OK;
    }
    
}


int registro_bytes_lixo(const Registro *r) {
    int bytes_ocupados = 0;
    // campos fixos
    bytes_ocupados = TAM_REMOVIDO + TAM_PROXIMO + TAM_COD_ESTACAO + TAM_COD_LINHA + TAM_COD_PROX_ESTACAO + TAM_DIST_PROX_ESTACAO + TAM_COD_LINHA_INTEGRA + TAM_COD_EST_INTEGRA + TAM_INDICE_NOME_ESTACAO + TAM_INDICE_NOME_LINHA;
    // campos variáveis
    bytes_ocupados += r->tamNomeEstacao + r->tamNomeLinha;

    return TAM_REGISTRO - bytes_ocupados;
}

RegistroStatus registro_escrever_bin(FILE *fp, const Registro *r) {
    // verificar ponteiros
    if (fp == NULL || r == NULL) {
        return REG_ERRO;
    }

    // escrever campos fixos
    if (fwrite(&r->removido, TAM_REMOVIDO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fwrite(&r->proximo, TAM_PROXIMO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fwrite(&r->codEstacao, TAM_COD_ESTACAO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fwrite(&r->codLinha, TAM_COD_LINHA, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fwrite(&r->codProxEstacao, TAM_COD_PROX_ESTACAO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fwrite(&r->distProxEstacao, TAM_DIST_PROX_ESTACAO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fwrite(&r->codLinhaIntegra, TAM_COD_LINHA_INTEGRA, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fwrite(&r->codEstIntegra, TAM_COD_EST_INTEGRA, 1, fp) != 1) {
        return REG_ERRO;
    }
    // escrever campos variáveis
    // verificar tamanho do nome da estação para escrever o índice e o nome
    if (fwrite(&r->tamNomeEstacao, TAM_INDICE_NOME_ESTACAO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (r->tamNomeEstacao > 0) {
        if (fwrite(r->nomeEstacao, sizeof(char), r->tamNomeEstacao, fp) != r->tamNomeEstacao) {
            return REG_ERRO;
        }
    }

    // verificar tamanho do nome da linha para escrever o índice e o nome
    if (fwrite(&r->tamNomeLinha, TAM_INDICE_NOME_LINHA, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (r->tamNomeLinha > 0) {
        if (fwrite(r->nomeLinha, sizeof(char), r->tamNomeLinha, fp) != r->tamNomeLinha) {
            return REG_ERRO;
        }
    }

    // escrever bytes de lixo para completar o registro
    int bytes_lixo = registro_bytes_lixo(r);
    char lixo[bytes_lixo];
    memset(lixo, BYTE_LIXO, bytes_lixo);
    if (fwrite(lixo, sizeof(char), bytes_lixo, fp) != bytes_lixo) {
        return REG_ERRO;
    }

    return REG_OK;
}

RegistroStatus registro_ler_bin(FILE *fp, Registro *r) {
    // verificar ponteiros
    if (fp == NULL || r == NULL) {
        return REG_ERRO;
    }

    // ler campos fixos
    if (fread(&r->removido, TAM_REMOVIDO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fread(&r->proximo, TAM_PROXIMO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fread(&r->codEstacao, TAM_COD_ESTACAO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fread(&r->codLinha, TAM_COD_LINHA, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fread(&r->codProxEstacao, TAM_COD_PROX_ESTACAO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fread(&r->distProxEstacao, TAM_DIST_PROX_ESTACAO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fread(&r->codLinhaIntegra, TAM_COD_LINHA_INTEGRA, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (fread(&r->codEstIntegra, TAM_COD_EST_INTEGRA, 1, fp) != 1) {
        return REG_ERRO;
    }
    
    // ler campos variáveis
    // ler índice do nome da estação para saber se tem nome e qual o tamanho
    if (fread(&r->tamNomeEstacao, TAM_INDICE_NOME_ESTACAO, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (r->tamNomeEstacao > 0) {
        r->nomeEstacao = (char *)malloc(r->tamNomeEstacao + 1);
        if (r->nomeEstacao == NULL) { // Verificar se a alocação foi bem sucedida
            return REG_ERRO;
        }
        if (fread(r->nomeEstacao, sizeof(char), r->tamNomeEstacao, fp) != r->tamNomeEstacao) {
            free(r->nomeEstacao);
            r->nomeEstacao = NULL;
            r->tamNomeEstacao = 0;
            return REG_ERRO;
        }
        // adicionar caractere nulo ao final da string lida
        r->nomeEstacao[r->tamNomeEstacao] = '\0';
    }else {
        r->nomeEstacao = NULL;
    }

    // ler índice do nome da linha para saber se tem nome e qual o tamanho
    if (fread(&r->tamNomeLinha, TAM_INDICE_NOME_LINHA, 1, fp) != 1) {
        return REG_ERRO;
    }
    if (r->tamNomeLinha > 0) {
        r->nomeLinha = (char *)malloc(r->tamNomeLinha + 1);
        if (r->nomeLinha == NULL) { // Verificar se a alocação foi bem sucedida
            return REG_ERRO;
        }
        if (fread(r->nomeLinha, sizeof(char), r->tamNomeLinha, fp) != r->tamNomeLinha) {
            free(r->nomeLinha);
            r->nomeLinha = NULL;
            r->tamNomeLinha = 0;
            return REG_ERRO;
        }
        // adicionar caractere nulo ao final da string lida
        r->nomeLinha[r->tamNomeLinha] = '\0';

    } else {
        r->nomeLinha = NULL;
    }
    //ler e descartar bytes de lixo
    if (registro_bytes_lixo(r) > 0) {
        char lixo[registro_bytes_lixo(r)];
        if (fread(lixo, sizeof(char), registro_bytes_lixo(r), fp) != registro_bytes_lixo(r)) {
            return REG_ERRO;
        }
    }
    return REG_OK;
}

void registro_imprimir(const Registro *r) {
    if (r == NULL) {
        printf("Registro nulo\n");
        return;
    }
    if (r->removido == '1') {
        printf("Registro removido\n");
        return;
    }
    if (r->codEstacao != -1) {
        printf("Codigo Estacao: %d\n", r->codEstacao);
    } else {
        printf("Codigo Estacao: Nulo\n");
    }
    if (r->codLinha != -1) {
        printf("Codigo Linha: %d\n", r->codLinha);
    } else {
        printf("Codigo Linha: Nulo\n");
    }
    if (r->codProxEstacao != -1) {
        printf("Codigo Proxima Estacao: %d\n", r->codProxEstacao);
    } else {
        printf("Codigo Proxima Estacao: Nulo\n");
    }
    if (r->distProxEstacao != -1) {
        printf("Distancia Proxima Estacao: %d\n", r->distProxEstacao);
    } else {
        printf("Distancia Proxima Estacao: Nulo\n");
    }
    if (r->codLinhaIntegra != -1) {
        printf("Codigo Linha Integracao: %d\n", r->codLinhaIntegra);
    } else {
        printf("Codigo Linha Integracao: Nulo\n");
    }
    if (r->codEstIntegra != -1) {
        printf("Codigo Estacao Integracao: %d\n", r->codEstIntegra);
    } else {
        printf("Codigo Estacao Integracao: Nulo\n");
    }
    if (r->nomeEstacao != NULL) {
        printf("Nome Estacao: %s\n", r->nomeEstacao);
    } else {
        printf("Nome Estacao: Nulo\n");
    }
    if (r->nomeLinha != NULL) {
        printf("Nome Linha: %s\n", r->nomeLinha);
    } else {
        printf("Nome Linha: Nulo\n");
    }
}



#include <assert.h>

#ifdef TEST_REGISTRO_BIN

int main(void) {
const char *arquivo = "teste_registro.bin";
Registro w = registro_start();
Registro r = registro_start();

/* monta registro de escrita */
w.removido = '0';
w.proximo = -1;
w.codEstacao = 10;
w.codLinha = 2;
w.codProxEstacao = 20;
w.distProxEstacao = 1500;
w.codLinhaIntegra = -1;
w.codEstIntegra = -1;

assert(registro_set_nome_estacao(&w, "Luz", 3) == REG_OK);
assert(registro_set_nome_linha(&w, "Azul", 4) == REG_OK);

/* escreve */
FILE *fp = fopen(arquivo, "wb");
assert(fp != NULL);
assert(registro_escrever_bin(fp, &w) == REG_OK);
fclose(fp);

/* valida tamanho físico do registro */
fp = fopen(arquivo, "rb");
assert(fp != NULL);
fseek(fp, 0, SEEK_END);
long tam = ftell(fp);
assert(tam == TAM_REGISTRO);
fclose(fp);

/* lê */
fp = fopen(arquivo, "rb");
assert(fp != NULL);
assert(registro_ler_bin(fp, &r) == REG_OK);
fclose(fp);

/* compara campos */
assert(r.removido == w.removido);
assert(r.proximo == w.proximo);
assert(r.codEstacao == w.codEstacao);
assert(r.codLinha == w.codLinha);
assert(r.codProxEstacao == w.codProxEstacao);
assert(r.distProxEstacao == w.distProxEstacao);
assert(r.codLinhaIntegra == w.codLinhaIntegra);
assert(r.codEstIntegra == w.codEstIntegra);

assert(r.tamNomeEstacao == w.tamNomeEstacao);
assert(r.tamNomeLinha == w.tamNomeLinha);
assert(strcmp(r.nomeEstacao, w.nomeEstacao) == 0);
assert(strcmp(r.nomeLinha, w.nomeLinha) == 0);
registro_imprimir(&r);
registro_free(&w);
registro_free(&r);

remove(arquivo);

printf("TESTE ESCREVER/LER BINARIO OK\n");
return 0;
}

#endif



