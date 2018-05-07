#include "aeroporto.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * aeroporto.c
 * Implementação das funções do aeroporto.h
 * Descrições em aeroporto.h
 **/

aeroporto_t* iniciar_aeroporto (size_t* args, size_t n_args) {
	aeroporto_t* aeroporto = (aeroporto_t*)malloc(sizeof(aeroporto_t));
	aeroporto->n_pistas = args[0];
	aeroporto->n_portoes = args[1];
	aeroporto->n_esteiras = args[2];
	aeroporto->n_max_avioes_esteira = args[3];
	aeroporto->t_pouso_decolagem = args[4];
	aeroporto->t_remover_bagagens = args[5];
	aeroporto->t_inserir_bagagens = args[6];
	aeroporto->t_bagagens_esteira = args[7];
	aeroporto->avioes = criar_fila(args[8]);
	sem_init(&(aeroporto->sem_pistas),0,args[0]);
	sem_init(&(aeroporto->sem_portoes),0,args[1]);
	sem_init(&(aeroporto->sem_esteiras),0,args[2]*args[3]);
	return aeroporto;
}

void aproximacao_aeroporto (aeroporto_t* aeroporto, aviao_t* aviao) {
	inserir(aeroporto->avioes, aviao);
	printf("Aviao %lu: Aproximando.\n", aviao->id);
	sem_wait(&aeroporto->sem_pistas);
	args_t* args = (args_t*)malloc(sizeof(args_t));
	args->aeroporto = (void*) aeroporto;
	args->aviao = (void*) remover(aeroporto->avioes);
	pthread_create(&(aviao->thread), NULL, rotina, (void *)args);
	return;
}

void* rotina (void* arg) {
	args_t* args = (args_t*)arg;
	aeroporto_t* aeroporto = (aeroporto_t*)args->aeroporto;
	aviao_t* aviao = (aviao_t*)args->aviao;
	free(args);  // free hugs
	pousar_aviao(aeroporto,aviao);
    pthread_exit(NULL);
}

void pousar_aviao (aeroporto_t* aeroporto, aviao_t* aviao) {
	printf("Aviao %lu: Pousando.\n", aviao->id);
	usleep(aeroporto->t_pouso_decolagem);
	printf("Aviao %lu: Pousou.\n", aviao->id);
	sem_post(&(aeroporto->sem_pistas));
	acoplar_portao(aeroporto,aviao);
	return;
}

void acoplar_portao (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&(aeroporto->sem_portoes));
	printf("Aviao %lu: Acoplado.\n", aviao->id);
	sem_post(&(aeroporto->sem_portoes));
	transportar_bagagens(aeroporto,aviao);
	return;
}

void transportar_bagagens (aeroporto_t* aeroporto, aviao_t* aviao) {
	printf("Aviao %lu: Removendo bagagens.\n", aviao->id);
	usleep(aeroporto->t_remover_bagagens);
	printf("Aviao %lu: Inserindo bagagens.\n", aviao->id);
	usleep(aeroporto->t_inserir_bagagens);
	printf("Aviao %lu: Bagagens transportadas.\n", aviao->id);
	adicionar_bagagens_esteira(aeroporto,aviao);
	return;
}

void adicionar_bagagens_esteira (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&(aeroporto->sem_esteiras));
	printf("Aviao %lu: Bagagens na esteira.\n", aviao->id);
	usleep(aeroporto->t_bagagens_esteira);
	printf("Aviao %lu: Bagagens sumiram da esteira.\n", aviao->id);
	usem_post(&(aeroporto->sem_esteiras));
	decolar_aviao(aeroporto,aviao);
	return;
}

void decolar_aviao (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&(aeroporto->sem_pistas));
	printf("Aviao %lu: Decolando.\n", aviao->id);
	usleep(aeroporto->t_pouso_decolagem);
	printf("Aviao %lu: Decolou.\n", aviao->id);
	sem_post(&(aeroporto->sem_pistas));
	desaloca_aviao(aviao);
	return;
}

int finalizar_aeroporto (aeroporto_t* aeroporto) {
	//pthread_join( thread2, NULL);
	desaloca_fila(aeroporto->avioes);
	sem_destroy(&(aeroporto->sem_pistas));
	sem_destroy(&(aeroporto->sem_portoes));
	sem_destroy(&(aeroporto->sem_esteiras));
	free(aeroporto); // seje livre, aeroporto-kun!
	return 0;
}
