#include <pthread.h>

struct estacao {
    pthread_mutex_t mutex;
    pthread_cond_t vagao_disponivel;
    pthread_cond_t embarque_concluido;
    int assentos_livres;
    int passageiros_esperando;
    int passageiros_embarcados;
    int passageiros_controle;
};

void estacao_init(struct estacao *estacao) {
    pthread_mutex_init(&estacao->mutex, NULL);
    pthread_cond_init(&estacao->vagao_disponivel, NULL);
    pthread_cond_init(&estacao->embarque_concluido, NULL);
    estacao->assentos_livres = 0;
    estacao->passageiros_esperando = 0;
    estacao->passageiros_embarcados = 0;
}

void estacao_preencher_vagao(struct estacao *estacao, int assentos) {
    pthread_mutex_lock(&estacao->mutex);
    estacao->assentos_livres = assentos;
    if(estacao->passageiros_esperando < estacao->assentos_livres){
        estacao->passageiros_embarcados = estacao->passageiros_esperando;
    }
    else{
        estacao->passageiros_embarcados = estacao->assentos_livres;
    }
    pthread_cond_broadcast(&estacao->vagao_disponivel);
    while (estacao->passageiros_esperando > 0 && estacao->assentos_livres > 0) {
        pthread_cond_wait(&estacao->embarque_concluido, &estacao->mutex);
    }
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_espera_pelo_vagao(struct estacao *estacao) {
    pthread_mutex_lock(&estacao->mutex);
    estacao->passageiros_esperando++;
    while (estacao->assentos_livres == 0 || estacao->passageiros_embarcados == 0) {
        pthread_cond_wait(&estacao->vagao_disponivel, &estacao->mutex);
    }
    estacao->passageiros_embarcados--;
    pthread_mutex_unlock(&estacao->mutex);
}

void estacao_embarque(struct estacao *estacao) {
    pthread_mutex_lock(&estacao->mutex);
    pthread_cond_signal(&estacao->embarque_concluido);
    estacao->passageiros_esperando--;
    estacao->assentos_livres--;
    pthread_mutex_unlock(&estacao->mutex);
}