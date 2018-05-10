#ifndef AEROPORTO_H
#define AEROPORTO_H

#include "aviao.h"
#include "fila.h"
#include <semaphore.h>

typedef size_t tempo_t;
typedef enum { false, true } bool;

typedef struct {
        bool ativo;
        size_t n_avioes;
	size_t n_pistas;
	size_t n_portoes;
	size_t n_esteiras;
	size_t n_max_avioes_esteira;
	tempo_t t_pouso_decolagem;
	tempo_t t_remover_bagagens;
	tempo_t t_inserir_bagagens;
	tempo_t t_bagagens_esteira;
        pthread_mutex_t mutex;
        pthread_mutex_t mutex_fila;
        pthread_mutex_t mutex_destruicao;
	sem_t sem_pistas;
	sem_t sem_portoes;
	sem_t sem_esteiras;
        sem_t sem_cons;
        sem_t sem_destrutor;
	fila_ordenada_t* avioes;
        aviao_t* omae_wa_mou_shindeiru;
} aeroporto_t;

typedef struct {
	void* aeroporto;
	void* aviao;
} args_t;

void* rotina (void* arg);

/**
 * Esta função deve fazer a alocação dinâmica de um aeroporto (malloc)
 * e atribuir os parâmetros (recebidos por um array de unsigned ints)
 * aos atributos da struct aeroporto
 **/
aeroporto_t* iniciar_aeroporto (size_t* args, size_t n_args);

void iniciar_aviao(aeroporto_t* aeroporto, size_t combustivel, size_t combustivel_min, size_t* id);
/**
 * Esta função deve ser chamada quando um novo avião se aproxima
 * do aeroporto. Nesta situação um avião deve pousar em seguida,
 * mas somente se houver uma pista livre para ele.
 **/
void* aproximacao_aeroporto (void* arg);//aeroporto_t* aeroporto, aviao_t* aviao);

/**
 * Esta função deve fazer com que o aviao pouse, utilizando uma pista livre.
 * Ela não pode ser chamada se não houver pistas livres. Em seguida o avião
 * deve se acoplar a um portão, caso haja um livre, se não houver, ele deve
 * esperar.
 **/
void pousar_aviao (aeroporto_t* aeroporto, aviao_t* aviao);

/**
 * Esta função deve acoplar um avião a um portão de embarque livre.
 * Ela não deve ser chamada caso não hajam portões livres. Em seguida, o
 * o avião deve transportar suas bagagens para fora e receber novas bagagens.
 * Um avião não pode decolar sem receber novas bagagens.
 **/
void acoplar_portao (aeroporto_t* aeroporto, aviao_t* aviao);

/**
 * Esta função deve levar as bagagens de um avião para uma esteira e
 * colocar novas bagagens num avião para que ele seja liberado para
 * decolagem. Um avião não pode decolar sem receber novas bagagens.
 * Um avião não pode receber novas bagagens sem colocar as antigas
 * em uma esteira.
 **/
void transportar_bagagens (aeroporto_t* aeroporto, aviao_t* aviao);

/**
 * Esta função deve colocar novas bagagens numa esteira, caso ela
 * esteja disponível. A esteira ficará ocupada enquanto houverem
 * bagagens nela.
 **/
void adicionar_bagagens_esteira (aeroporto_t* aeroporto, aviao_t* aviao);

/**
 * Esta função deve fazer com que um avião decole, caso haja
 * uma pista disponível para tal. Um avião que decolar deve
 * ter sua execução terminada.
 **/
void decolar_aviao (aeroporto_t* aeroporto, aviao_t* aviao);

void* destruir_aviao(void* arg);

/**
 * Esta função deve desalocar todos os recursos previamente
 * alocados pelo aeroporto. Retorna 1 caso bem sucedido.
 **/
int finalizar_aeroporto (aeroporto_t* aeroporto);

#endif
