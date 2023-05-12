#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "copy.c"

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

int run_test(int numPassageiros, int numAssentos){

  struct estacao estacao;
  estacao_init(&estacao);

  int passageiro = numPassageiros;

  pthread_t passageiros[numPassageiros];

  for(int i = 0; i < numPassageiros; i++){
    //printf("Passageiro %d chegou\n", i+1);
    pthread_create(&passageiros[i], NULL, (void *)passageiros_thread, (void *)&estacao);
  }     
    sleep(1);

  struct vagao_args vargs;
  vargs.estacao = &estacao;  
  vargs.assentos_livres = numAssentos;
  
  while(passageiro > 0){
    int assentos = numAssentos;

    pthread_t vagao;
    pthread_create(&vagao, NULL, (void *)vagao_thread, (void *)&vargs);
    //printf("Vagão %d chegou na estação\n", repCount++);

    int reap;
    if(passageiro < assentos){
      reap = passageiro;
    }else {
      reap = assentos;
    }
    while(reap != 0){
        if (counter > 0){
            estacao_embarque(&estacao);
            __atomic_fetch_add(&counter, -1, __ATOMIC_SEQ_CST);
            passageiro--;
            assentos--;
            reap--;
        }
    }
    if (counter != 0){
      printf("%d\n",counter);
      printf("Deu Ruim irmão, tente novamente.\n");
      exit(0);
    } 
    printf("Vagão saiu da estação\n");
    }
    printf("Estação finalizada\n");
  return 0;
}

int main(void){
  run_test(51, 50);

}