#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "metrorec.c"
// #define maxAssentos
int counter = 0;

void *passageiros_thread(void *arg)
{
  struct estacao *estacao = (struct estacao *) arg;
	estacao_espera_pelo_vagao(estacao);

   __atomic_fetch_add(&counter, 1, __ATOMIC_SEQ_CST);
  
	return NULL;
}

struct vagao_args {
	struct estacao *estacao;
	int assentos_livres;
};

void *vagao_thread(void *args)
{
	struct vagao_args *vargs = (struct vagao_args *) args;
	estacao_preencher_vagao(vargs->estacao, vargs->assentos_livres);
	return NULL;
}

int run_test(int numPassageiros, int maxAssentos) {
    struct estacao estacao;
    estacao_init(&estacao);
    int passageiro = numPassageiros;
    pthread_t passageiros[numPassageiros];

    for (int i = 0; i < numPassageiros; i++) {
        pthread_create(&passageiros[i], NULL, (void *)passageiros_thread, (void *)&estacao);
    }
    sleep(1);

    while (passageiro > 0) {
        int assentos = rand() % maxAssentos + 1;  // gerar número aleatório de assentos
        printf("[-] Numero %d Passageiros\n", passageiro);
        printf("[-] Numero %d Assentos\n", assentos);


        if (assentos == 0) {
          assentos++;
        }
        struct vagao_args vargs = {&estacao, assentos};

        pthread_t vagao;
        pthread_create(&vagao, NULL, (void *)vagao_thread, (void *)&vargs);

        int reap = (passageiro < assentos) ? passageiro : assentos;
        while (reap != 0) {
            if (counter > 0) {
                estacao_embarque(&estacao);
                __atomic_fetch_add(&counter, -1, __ATOMIC_SEQ_CST);
                passageiro--;
                assentos--;
                reap--;
            }
        }
        if (counter != 0) {
            printf("%d\n", counter);
            printf("Deu Ruim irmão, tente novamente.\n");
            exit(0);
        } 

        

        printf("[+] Vagão saiu da estação com %d assentos - Sobrou %d Passageiros\n", assentos, passageiro);
    }
    printf("\n=====================================\n");
    printf("Estação finalizada\n");
    sleep(1);
    return 0;
}


int main(void){
  for (int i = 0; i < 10; i++) {
    printf("[-] Teste %d\n", i);
    int random = rand() % 2000 + 1;
    run_test(random,1000);
  }

  printf("\n[-]=====================================[-]\n");
  printf("Finalizou\n");

}