#include "fila.h"
#include <stdlib.h>

/**
 * fila.c
 * Implementação das funções da fila.h
 * Descrições em fila.h
 **/

elemento_t* aloca_elemento (aviao_t* dado) {
    elemento_t* elemento = (elemento_t*)malloc(sizeof(elemento_t));
    elemento->dado = dado;
    return elemento;
}

void desaloca_elemento (elemento_t* elemento) {
    free(elemento);  // seje livre!
    return; //elemento is free!
}

fila_ordenada_t* criar_fila(size_t pouco_combustivel) {
    fila_ordenada_t* fila = (fila_ordenada_t*)malloc(sizeof(fila_ordenada_t));
    fila->primeiro = NULL;
    fila->ultimo = NULL;
    fila->n_elementos = 0;
    fila->pouco_combustivel = pouco_combustivel;
    return fila;
}

void desaloca_fila (fila_ordenada_t * fila) {
    while (fila->n_elementos > 0) {
        free(remover(fila));
    }
    free(fila);
    return;
}

void inserir (fila_ordenada_t* fila, aviao_t* dado) {
    if(fila->n_elementos == 0) {
        elemento_t* elemento = aloca_elemento(dado);
        fila->primeiro = elemento;
        fila->ultimo = elemento;
        fila->n_elementos++;
        return;
    }
    elemento_t* atual = fila->ultimo;
    if(dado->combustivel <= fila->pouco_combustivel) {
        while(atual->dado->combustivel > fila->pouco_combustivel){
            if(atual == fila->primeiro){
                break;  // dance
            }
            atual = atual->anterior;
        }
    }
    elemento_t* elemento = aloca_elemento(dado);
    if(atual->proximo == fila->ultimo) {
        fila->ultimo = elemento;
    }
    atual->proximo = elemento;
    fila->n_elementos++;
    return;
}

aviao_t* remover (fila_ordenada_t* fila) {
    if(fila->n_elementos == 0){
        return NULL;
    }
    if(fila->n_elementos == 1){
        aviao_t* aviao = fila->primeiro->dado;
        desaloca_elemento(fila->primeiro);
        fila->primeiro = NULL;
        fila->ultimo = NULL;
        fila->n_elementos--;
        return aviao;
    }

    elemento_t* elemento = fila->primeiro;
    fila->primeiro = elemento->proximo;
    elemento->proximo->anterior = NULL;
    aviao_t* aviao = elemento->dado;
    desaloca_elemento(elemento);
    fila->n_elementos--;
    return aviao;
}
