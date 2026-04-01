#include "../headers/registro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#include <assert.h>

#ifdef TEST_REGISTRO

int main(void) {
Registro r = registro_start();
assert(r.removido == '0');
assert(r.proximo == -1);
assert(r.nomeEstacao == NULL);
assert(r.tamNomeEstacao == 0);
assert(r.nomeLinha == NULL);
assert(r.tamNomeLinha == 0);

assert(registro_set_nome_estacao(&r, "Luz", 3) == REG_OK);
assert(strcmp(r.nomeEstacao, "Luz") == 0);
assert(r.tamNomeEstacao == 3);

assert(registro_set_nome_estacao(&r, "Consolacao", 10) == REG_OK);
assert(strcmp(r.nomeEstacao, "Consolacao") == 0);
assert(r.tamNomeEstacao == 10);

assert(registro_set_nome_linha(&r, "Azul", 4) == REG_OK);
assert(strcmp(r.nomeLinha, "Azul") == 0);
assert(r.tamNomeLinha == 4);

assert(registro_set_nome_linha(&r, NULL, 0) == REG_OK);
assert(r.nomeLinha == NULL);
assert(r.tamNomeLinha == 0);

assert(registro_set_nome_estacao(&r, "Luz", 2) == REG_ERRO);

registro_free(&r);
assert(r.nomeEstacao == NULL);
assert(r.tamNomeEstacao == 0);
assert(r.nomeLinha == NULL);
assert(r.tamNomeLinha == 0);

printf("TESTE REGISTRO OK\n");
return 0;
}

#endif



