#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>

#include "aeroporto.h"

#define NOVO_AVIAO_MIN 30
#define NOVO_AVIAO_MAX 120
#define COMBUSTIVEL_MIN 1
#define COMBUSTIVEL_MAX 10
#define TEMPO_POUSO_DECOLAGEM 40
#define TEMPO_REMOVER_BAGAGENS 90
#define TEMPO_INSERIR_BAGAGENS 110
#define TEMPO_BAGAGENS_ESTEIRA 200
#define TEMPO_SIMULACAO 10000

int main(int argc, char** argv) {

    // Variáveis temporais (inicio t_)
    size_t t_novo_aviao_min, t_novo_aviao_max;
    size_t t_pouso_decolagem;
    size_t t_remover_bagagens, t_inserir_bagagens;
    size_t t_bagagens_esteira, t_simulacao;

    // Variáveis discretas (inicio n_)
    size_t n_pistas, n_portoes;
    size_t n_max_avioes_esteira, n_esteiras;
    size_t n_args;

    // Variáveis de prioridade (inicio p_)
    size_t p_combustivel_min, p_combustivel_max;

    if (argc == 5) { // Argumentos sem tempos de execução
        t_novo_aviao_min = NOVO_AVIAO_MIN;
        t_novo_aviao_max = NOVO_AVIAO_MAX;
        t_pouso_decolagem = TEMPO_POUSO_DECOLAGEM;
        t_remover_bagagens = TEMPO_REMOVER_BAGAGENS;
        t_inserir_bagagens = TEMPO_INSERIR_BAGAGENS;
        t_bagagens_esteira = TEMPO_BAGAGENS_ESTEIRA;
        t_simulacao = TEMPO_SIMULACAO;
        p_combustivel_min = COMBUSTIVEL_MIN;
        p_combustivel_max = COMBUSTIVEL_MAX;
        n_pistas = atoi(argv[1]);
        n_portoes = atoi(argv[2]);
        n_max_avioes_esteira = atoi(argv[3]);
        n_esteiras = atoi(argv[4]);

    } else if (argc == 14) { // Argumentos com tempos de execução
        int i = 0; // Este contador será incrementado antes de coletar um argumento
        t_novo_aviao_min = atoi(argv[++i]);
        t_novo_aviao_max = atoi(argv[++i]);
        p_combustivel_min = atoi(argv[++i]);
        p_combustivel_max = atoi(argv[++i]);
        t_pouso_decolagem = atoi(argv[++i]);
        n_pistas = atoi(argv[++i]);
        t_remover_bagagens = atoi(argv[++i]);
        t_inserir_bagagens = atoi(argv[++i]);
        n_portoes = atoi(argv[++i]);
        n_max_avioes_esteira = atoi(argv[++i]);
        t_bagagens_esteira = atoi(argv[++i]);
        n_esteiras = atoi(argv[++i]);
        t_simulacao = atoi(argv[++i]);

    } else { // Número incorreto de argumentos
        printf("Todas as entradas são inteiros positivos!!\nUso:\n");
        printf("./aeroporto  NOVO_AVIAO_MIN  NOVO_AVIAO_MAX\n");
        printf("COMBUSTIVEL_MIN COMBUSTIVEL_MAX\n");
        printf("TEMPO_POUSO_DECOLAGEM  NUMERO_PISTAS  TEMPO_REMOVER_BAGAGENS\n");
        printf("TEMPO_INSERIR_BAGAGENS  NUMERO_PORTOES  MAXIMO_AVIOES_ESTEIRA\n");
        printf("TEMPO_BAGAGENS_ESTEIRA  NUMERO_ESTEIRAS  TEMPO_SIMULACAO\n");
        printf("----------OU----------\n");
        printf("./airport  NUMERO_PISTAS  NUMERO_PORTOES  MAXIMO_AVIOES_ESTEIRA  NUMERO_ESTEIRAS\n");
        return 0;
    }

    // Impressão com os parâmetros selecionados para simulação
    printf("Simulação iniciada com tempo total: %zu\n", t_simulacao);
    printf("Tempo para criação de aviões: %zu - %zu\n", t_novo_aviao_min, t_novo_aviao_max);
    printf("Número de pistas de pouso: %zu\n", n_pistas);
    printf("Tempo de pouso e decolagem: %zu\n", t_pouso_decolagem);
    printf("Número de portões de embarque: %zu\n", n_portoes);
    printf("Tempo de inserção (%zu) e remoção (%zu) de bagagens\n", t_inserir_bagagens, t_remover_bagagens);
    printf("Número de esteiras: %zu, com %zu aviões por esteira\n", n_esteiras, n_max_avioes_esteira);
    printf("Tempo das bagagens nas esteiras: %zu\n", t_bagagens_esteira);

    // Inicialização do aeroporto
    n_args = 9;
    size_t args[9] = {n_pistas, n_portoes, n_esteiras,
            n_max_avioes_esteira,
            t_pouso_decolagem, t_remover_bagagens,
            t_inserir_bagagens, t_bagagens_esteira,
            p_combustivel_max / 10};

    aeroporto_t* meu_aeroporto = iniciar_aeroporto(args, n_args);

    size_t p_combustivel = p_combustivel_max - p_combustivel_min;
    size_t id = 0;

    pthread_t thread_aproxima;
    pthread_create(&thread_aproxima, NULL, aproximacao_aeroporto,
            (void *) meu_aeroporto);

    pthread_t thread_destroi;
    pthread_create(&thread_destroi, NULL, destruir_aviao,
            (void *) meu_aeroporto);

    srand(time(NULL));
    double diff = 0.0;
    time_t start;
    time_t stop;
    time_t start_aviao;
    time_t stop_aviao;
    time(&start);
    time(&start_aviao);
    time_t t_novo_aviao = t_novo_aviao_min - t_novo_aviao_max;
    time_t t_n_aviao = 0;
    while ( diff < t_simulacao ) {
        time(&stop_aviao);
        if (difftime(stop_aviao, start_aviao) * 10000 > t_n_aviao/10) {
            iniciar_aviao(meu_aeroporto, p_combustivel, p_combustivel_min, &id);
            time(&start_aviao);
            t_n_aviao = t_novo_aviao_min + (rand() % t_novo_aviao);
        }

        time(&stop);
        diff = difftime(stop, start) * 1000;
    }

    printf("encerrando simulacao\n");
    meu_aeroporto->ativo = false;
    printf("Aguardando %zu avioes\n", meu_aeroporto->n_avioes);
    pthread_join(thread_aproxima, NULL);
    pthread_join(thread_destroi, NULL);

    finalizar_aeroporto(meu_aeroporto);
    return 1;
}
