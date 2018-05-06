#include "aeroporto.h"
#include <semaphore.h>
#include <unistd.h>

/**
 * aeroporto.c
 * Implementação das funções do aeroporto.h
 * Descrições em aeroporto.h
 **/

aeroporto_t* iniciar_aeroporto (size_t* args, size_t n_args) {
	aeroporto_t* aeroporto = malloc(sizeof(aeroporto_t));
	aeroporto->n_pistas = args[0];
	aeroporto->n_portoes = args[1];
	aeroporto->n_esteiras = args[2];
	aeroporto->n_max_avioes_esteira = args[3];
	aeroporto->t_pouso_decolagem = args[4];
	aeroporto->t_remover_bagagens = args[5];
	aeroporto->t_inserir_bagagens = args[6];
	aeroporto->t_bagagens_esteira = args[7];
	sem_init(&aeroporto->sem_pistas,0,args[0]);
	sem_init(&aeroporto->sem_portoes,0,args[1]);
	sem_init(&aeroporto->sem_esteiras,0,args[2]*args[3]);
	return aeroporto;
}

void aproximacao_aeroporto (aeroporto_t* aeroporto, aviao_t* aviao) {
	
	pthread_create(&aviao->thread, NULL, rotina, (void *)&num_threads);
	printf("Aviao %d: Aproximando.\n", aviao->id);
	pousar_aviao(aeroporto,aviao);
	return;
}

void pousar_aviao (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&aeroporto->sem_pistas);
	printf("Aviao %d: Pousando.\n", aviao->id);
	sleep(aeroporto->t_pouso_decolagem);
	printf("Aviao %d: Pousou.\n", aviao->id);
	sem_post(&aeroporto->sem_pistas);
	acoplar_portao(aeroporto,aviao);
	return;
}

void acoplar_portao (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&aeroporto->sem_portoes);
	printf("Aviao %d: Acoplado.\n", aviao->id);
	sem_post(&aeroporto->sem_portoes);
	transportar_bagagens(aeroporto,aviao);
	return;
}

void transportar_bagagens (aeroporto_t* aeroporto, aviao_t* aviao) {
	printf("Aviao %d: Removendo bagagens.\n", aviao->id);
	sleep(aeroporto->t_remover_bagagens);
	printf("Aviao %d: Inserindo bagagens.\n", aviao->id);
	sleep(aeroporto->t_inserir_bagagens);
	printf("Aviao %d: Bagagens transportadas.\n", aviao->id);
	adicionar_bagagens_esteira(aeroporto,aviao);
	return;
}

void adicionar_bagagens_esteira (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&aeroporto->sem_esteiras);
	printf("Aviao %d: Bagagens na esteira.\n", aviao->id);
	sleep(aeroporto->t_bagagens_esteira);
	printf("Aviao %d: Bagagens sumiram da esteira.\n", aviao->id);
	sem_post(&aeroporto->sem_esteiras);
	decolar_aviao(aeroporto,aviao);
	return;
}

void decolar_aviao (aeroporto_t* aeroporto, aviao_t* aviao) {
	sem_wait(&aeroporto->sem_pistas);
	printf("Aviao %d: Decolando.\n", aviao->id);
	sleep(aeroporto->t_pouso_decolagem);
	printf("Aviao %d: Decolou.\n", aviao->id);
	sem_post(&aeroporto->sem_pistas);
	desaloca_aviao(aviao);
	return;
}

int finalizar_aeroporto (aeroporto_t* aeroporto) {
	sem_destroy(&aeroporto->sem_pistas);
	sem_destroy(&aeroporto->sem_portoes);
	sem_destroy(&aeroporto->sem_esteiras);
	pthread_join( thread2, NULL);
	free(aeroporto); // seje livre, aeroporto-kun!
	printf("3 avioes restantes foram derrubados.\n");
	return 0;
}
