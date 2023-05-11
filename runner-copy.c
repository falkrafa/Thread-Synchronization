#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "copy.c"

int retornoCont = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *passageiros_thread(void *arg)
{
    struct estacao *estacao = (struct estacao *)arg;
    estacao_espera_pelo_vagao(estacao);

    pthread_mutex_lock(&mutex);
    retornoCont++;
    pthread_mutex_unlock(&mutex);
    // printf("RETORNOU!, valor compartilhado: %d\n", retornoCont);

    return NULL;
}

struct vagao_args
{
    struct estacao *estacao;
    int assentos_livres;
};

void *vagao_thread(void *args)
{
    struct vagao_args *vargs = (struct vagao_args *)args;
    estacao_preecher_vagao(vargs->estacao, vargs->assentos_livres);
    return NULL;
}

void run_test(int num_passageiros, int num_assentos)
{
    struct estacao estacao;
    estacao_init(&estacao);

    pthread_t passageiros[num_passageiros];

    for (int i = 0; i < num_passageiros; i++)
    {
        pthread_create(&passageiros[i], NULL, passageiros_thread, &estacao);
    }

    while (num_passageiros > 0)
    {
        struct vagao_args vargs;
        vargs.estacao = &estacao;
        vargs.assentos_livres = num_assentos;

        pthread_t vagao;
        pthread_create(&vagao, NULL, vagao_thread, &vargs);

        int numPassengersReap = (num_passageiros > num_assentos) ? num_assentos : num_passageiros;
        num_passageiros -= numPassengersReap;

        for (int i = 0; i < numPassengersReap; i++)
        {
            while (retornoCont != numPassengersReap)
                ;
            // aguarda o retorno das threads de passageiros
            estacao_embarque(&estacao);
        }

        pthread_mutex_lock(&mutex);
        retornoCont = 0;
        pthread_mutex_unlock(&mutex);
    }
}

int main(void)
{
    run_test(30, 30);
    // passageiros, assentos
    return 0;
}