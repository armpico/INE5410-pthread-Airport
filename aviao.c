#include "aviao.h"

/**
 * aviao.c
 * Implementação das funções do aviao.h
 * Descrições em aviao.h
 **/

aviao_t* aloca_aviao (size_t combustivel, size_t id) {
    aviao_t* aviao = malloc(sizeof(aviao_t));
    aviao->combustivel = combustivel;
    aviao->id = id;
    pthread_create(&aviao->thread, NULL, rotina, (void *)&num_threads);
    return aviao;
}

void desaloca_aviao(aviao_t* aviao) {
    
    free(aviao);
    return;
}
