#include "TADS/headers/registro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void registro_start(Registro *r) {
    // Verificar ponteiro nulo
    if (r == NULL) {
        fprintf(stderr, "Erro: ponteiro para registro é nulo em registro_start.\n");
        return;
    }
    r->removido = '0';
    r->proximo = -1;
    r->codEstacao = -1;
    r->codLinha = -1;
    r->codProxEstacao = -1;
    r->distProxEstacao = -1;
    r->codLinhaIntegra = -1;
    r->codEstIntegra = -1;
    r->tamNomeEstacao = 0;
    r->nomeEstacao = NULL;
    r->tamNomeLinha = 0;
    r->nomeLinha = NULL;
}

void registro_free(Registro *r) {
    if (r == NULL) {
        fprintf(stderr, "Erro: ponteiro para registro é nulo em registro_free.\n");
        return;
    }
    if (r->nomeEstacao != NULL) {
        free(r->nomeEstacao);
        r->nomeEstacao = NULL;
    }
    if (r->nomeLinha != NULL) {
        free(r->nomeLinha);
        r->nomeLinha = NULL;
    }
}

