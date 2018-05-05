#include "fila.h"

/**
 * fila.c
 * Implementação das funções da fila.h
 * Descrições em fila.h
 **/

elemento_t* aloca_elemento (aviao_t * dado) {
    elemento_t* elemento = malloc(sizeof(elemento_t));
    elemento->dado = dado;
    return elemento;
}

void desaloca_elemento (elemento_t * elemento) {
    free(elemento);  // seje livre!
    return; //elemento is free!
}

fila_ordenada_t * criar_fila () {
    fila_ordenada_t* fila = malloc(sizeof(fila_ordenada_t));
    fila->primeiro = NULL;
    fila->ultimo = NULL;
    fila->n_elementos = 0;
    return fila;
}

void desaloca_fila (fila_ordenada_t * fila) {
    free(fila);
    return;
}

void inserir (fila_ordenada_t * fila, aviao_t * dado) {
    if(dado->combustivel == 1) {

    }
    return;
}

aviao_t * remover (fila_ordenada_t * fila) {
    aviao_t aviao = fila->primeiro;
    fila->primeiro = aviao->proximo;
    aviao->proximo->anterior = NULL;
    return aviao;
}
