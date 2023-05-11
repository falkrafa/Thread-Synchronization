#include <pthread.h>
#include <stdio.h>

struct estacao {
    pthread_mutex_t mutex;
    pthread_cond_t vagao_disponivel;
    pthread_cond_t embarque_concluido;
    int assentos_livres;
    int passageiros_esperando;
    int passageiros_embarcados;
};

void estacao_init(struct estacao *estacao) {
    pthread_mutex_init(&estacao->mutex, NULL);
    pthread_cond_init(&estacao->vagao_disponivel, NULL);
    pthread_cond_init(&estacao->embarque_concluido, NULL);
    estacao->assentos_livres = 0;
    estacao->passageiros_esperando = 0;
    estacao->passageiros_embarcados = 0;
    printf("Estação inicializada.\n");
}

void estacao_preecher_vagao(struct estacao *estacao, int assentos) {
    pthread_mutex_lock(&estacao->mutex);
    estacao->assentos_livres = assentos;
    printf("vagao chegou\n");
    pthread_cond_broadcast(&estacao->vagao_disponivel);

    while (estacao->passageiros_embarcados < estacao->assentos_livres ) {
        pthread_cond_wait(&estacao->embarque_concluido, &estacao->mutex);
        estacao->assentos_livres--;
        estacao->passageiros_embarcados++;
    }

    pthread_mutex_unlock(&estacao->mutex);
}


void estacao_espera_pelo_vagao(struct estacao *estacao) {
    pthread_mutex_lock(&estacao->mutex);

    printf("Passageiro %d criado.\n", estacao->passageiros_esperando);
    estacao->passageiros_esperando++;

    while (estacao->assentos_livres == 0 || estacao->passageiros_embarcados == estacao->assentos_livres) {
        pthread_cond_wait(&estacao->vagao_disponivel, &estacao->mutex);
    }

    estacao->passageiros_esperando--;
    estacao->passageiros_embarcados++;

    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_embarque(struct estacao *estacao) {
    pthread_mutex_lock(&estacao->mutex);
    printf("Passageiro embarcando.\n");
    if (estacao->passageiros_embarcados == estacao->assentos_livres) {
        pthread_cond_broadcast(&estacao->embarque_concluido);
    }
    else{
        pthread_cond_signal(&estacao->embarque_concluido);
    }
    pthread_mutex_unlock(&estacao->mutex);
}