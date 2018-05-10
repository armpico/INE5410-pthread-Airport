#include "aeroporto.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * aeroporto.c
 * Implementação das funções do aeroporto.h
 * Descrições em aeroporto.h
 **/
#define INCONSTANTE_DE_TEMPO 10000

aeroporto_t* iniciar_aeroporto(size_t* args, size_t n_args) {
    aeroporto_t* aeroporto = (aeroporto_t*) malloc(sizeof (aeroporto_t));
    aeroporto->ativo = true;
    aeroporto->n_avioes = 0;
    aeroporto->n_pistas = args[0];
    aeroporto->n_portoes = args[1];
    aeroporto->n_esteiras = args[2];
    aeroporto->n_max_avioes_esteira = args[3];
    aeroporto->t_pouso_decolagem = args[4];
    aeroporto->t_remover_bagagens = args[5];
    aeroporto->t_inserir_bagagens = args[6];
    aeroporto->t_bagagens_esteira = args[7];
    aeroporto->avioes = criar_fila(args[8]);
    pthread_mutex_init(&(aeroporto->mutex), NULL);
    pthread_mutex_init(&(aeroporto->mutex_fila), NULL);
    pthread_mutex_init(&(aeroporto->mutex_destruicao), NULL);
    sem_init(&(aeroporto->sem_pistas), 0, args[0]);
    sem_init(&(aeroporto->sem_portoes), 0, args[1]);
    sem_init(&(aeroporto->sem_esteiras), 0, args[2] * args[3]);
    sem_init(&(aeroporto->sem_cons), 0, 0);
    sem_init(&(aeroporto->sem_destrutor), 0, 0);
    return aeroporto;
}

void iniciar_aviao(aeroporto_t* aeroporto, size_t p_combustivel, size_t combustivel_min, size_t* id) {
    pthread_mutex_lock(&(aeroporto->mutex));
    aeroporto->n_avioes++;
    pthread_mutex_unlock(&(aeroporto->mutex));

    size_t combustivel = combustivel_min + (rand() % p_combustivel);
    aviao_t* aviao = aloca_aviao(combustivel, (*id)++, aeroporto);

    pthread_mutex_lock(&(aeroporto->mutex_fila));
    inserir(aeroporto->avioes, aviao);
    pthread_mutex_unlock(&(aeroporto->mutex_fila));

    printf("Aviao %zu: Aproximando com %zu de combustivel.\n", aviao->id, aviao->combustivel);
    sem_post(&(aeroporto->sem_cons));
}

void* aproximacao_aeroporto(void* arg) {
    aeroporto_t* aeroporto = (aeroporto_t*)arg;
    while(aeroporto->ativo) {
        aviao_t* aviao = NULL;
        sem_wait(&(aeroporto->sem_cons));
        while (aviao == NULL) {
            pthread_mutex_lock(&(aeroporto->mutex_fila));
            aviao = remover(aeroporto->avioes);
            pthread_mutex_unlock(&(aeroporto->mutex_fila));
        }

        sem_wait(&(aeroporto->sem_pistas));

        args_t* args = (args_t*) malloc(sizeof (args_t));
        args->aeroporto = (void*) aeroporto;
        args->aviao = (void*) aviao;
        pthread_create(&(aviao->thread), NULL, rotina, (void *) args);

    }
    pthread_exit(NULL);
}

void* rotina(void* arg) {
    args_t* args = (args_t*) arg;
    aeroporto_t* aeroporto = (aeroporto_t*) args->aeroporto;
    aviao_t* aviao = (aviao_t*) args->aviao;
    free(args); // free hugs
    pousar_aviao(aeroporto, aviao);
    pthread_exit(NULL);
}

void pousar_aviao(aeroporto_t* aeroporto, aviao_t* aviao) {
    printf("Aviao %zu: Pousando.\n", aviao->id);
    usleep(aeroporto->t_pouso_decolagem * INCONSTANTE_DE_TEMPO);
    printf("Aviao %zu: Pousou.\n", aviao->id);
    sem_post(&(aeroporto->sem_pistas));
    acoplar_portao(aeroporto, aviao);
    return;
}

void acoplar_portao(aeroporto_t* aeroporto, aviao_t* aviao) {
    sem_wait(&(aeroporto->sem_portoes));
    printf("Aviao %zu: Acoplado.\n", aviao->id);
    sem_post(&(aeroporto->sem_portoes));
    transportar_bagagens(aeroporto, aviao);
    return;
}

void transportar_bagagens(aeroporto_t* aeroporto, aviao_t* aviao) {
    printf("Aviao %zu: Removendo bagagens.\n", aviao->id);
    usleep(aeroporto->t_remover_bagagens * INCONSTANTE_DE_TEMPO);
    printf("Aviao %zu: Inserindo bagagens.\n", aviao->id);
    usleep(aeroporto->t_inserir_bagagens * INCONSTANTE_DE_TEMPO);
    printf("Aviao %zu: Bagagens transportadas.\n", aviao->id);
    adicionar_bagagens_esteira(aeroporto, aviao);
    return;
}

void adicionar_bagagens_esteira(aeroporto_t* aeroporto, aviao_t* aviao) {
    sem_wait(&(aeroporto->sem_esteiras));
    printf("Aviao %zu: Bagagens na esteira.\n", aviao->id);
    usleep(aeroporto->t_bagagens_esteira * INCONSTANTE_DE_TEMPO);
    printf("Aviao %zu: Bagagens sumiram da esteira.\n", aviao->id);
    sem_post(&(aeroporto->sem_esteiras));
    decolar_aviao(aeroporto, aviao);
    return;
}

void decolar_aviao(aeroporto_t* aeroporto, aviao_t* aviao) {
    sem_wait(&(aeroporto->sem_pistas));
    size_t id = aviao->id;
    printf("Aviao %zu: Decolando.\n", aviao->id);
    usleep(aeroporto->t_pouso_decolagem * INCONSTANTE_DE_TEMPO);
    printf("Aviao %zu: Decolou.\n", id);
    sem_post(&(aeroporto->sem_pistas));

    pthread_mutex_lock(&(aeroporto->mutex_destruicao));
    aeroporto->omae_wa_mou_shindeiru = aviao;
    sem_post(&(aeroporto->sem_destrutor));

    return;
}

void* destruir_aviao(void* arg) {
    aeroporto_t* aeroporto = (aeroporto_t*)arg;
    bool continuar = true;
    while(continuar) {
        sem_wait(&(aeroporto->sem_destrutor));

        desaloca_aviao(aeroporto->omae_wa_mou_shindeiru);
        pthread_mutex_unlock(&(aeroporto->mutex_destruicao));

        pthread_mutex_lock(&(aeroporto->mutex));
        aeroporto->n_avioes--;
        continuar = aeroporto->ativo || aeroporto->n_avioes > 0;
        pthread_mutex_unlock(&(aeroporto->mutex));
    }
    pthread_exit(NULL);
}

int finalizar_aeroporto(aeroporto_t* aeroporto) {
    //pthread_join( thread2, NULL);
    desaloca_fila(aeroporto->avioes);
    sem_destroy(&(aeroporto->sem_pistas));
    sem_destroy(&(aeroporto->sem_portoes));
    sem_destroy(&(aeroporto->sem_esteiras));
    free(aeroporto);
    return 0;
}
