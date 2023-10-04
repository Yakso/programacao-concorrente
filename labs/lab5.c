/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Paulo Yamagishi  DRE: 12107893 */
/* Codigo: A thread main (produtora) cria N threads (consumidoras) e depois le linha a
linha o conteudo de um arquivo texto (nome do arquivo informado na linha de comando)
e as deposita em um buffer compartilhado (preservando a ordem do texto). As threads
devem retirar as linhas do buffer e imprimi-las na saida padrao (mantendo a ordem do texto).*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define QTD_LINHAS 10000000 //quantidade de linhas do arquivo

// Variaveis globais
char Buffer[QTD_LINHAS]; //buffer compartilhado
char linhaRemovida[100]; //linha removida do buffer
int nthreads; //número de threads  
sem_t slotCheio, slotVazio; //semaforos para controle de slots cheios e vazios
sem_t mutexCons; //semaforos para exclusao mutua entre as threads   

void Insere (char *linha) {
  static int in = 0;
  //aguarda slot vazio
  sem_wait(&slotVazio);
  //insere linha do arquivo no buffer
  strcpy(Buffer[in], linha);
  printf("Linha inserida: %s", Buffer[in]);
  in = (in+1)%QTD_LINHAS;
  //incrementa a qtde de slots cheios
  sem_post(&slotCheio);
}

void Remove (void) {
  static int out = 0;
  //aguarda slot cheio
  sem_wait(&slotCheio);
  //exclusao mutua entre consumidores
  sem_wait(&mutexCons);
  Buffer[out] = '\0';
  out = (out+1)%QTD_LINHAS;
  //libera exclusao mutua entre consumidores
  sem_post(&mutexCons);
  //libera slot vazio 
  sem_post(&slotVazio);
}

void produtor(char *arg) {
    char linha[100];
    FILE *arq;
    arq = fopen(arg, "r");
    //le linha a linha do arquivo
    while(fgets(linha, 100, arq) != NULL) {
        //printf("Linha lida: %s", linha);
        Insere(linha);
    }
    fclose(arq);
    pthread_exit(NULL);
}

//funcao executada pelas threads
void *consumidor(void *arg) {
    //remove linha do buffer e imprime na saida padrao
    while(1) {
        Remove();
    }
    pthread_exit(NULL);
}


//funcao principal
int main(int argc, char *argv[]) {
  pthread_t *tid;
  int t, *id; 

  //verifica se o numero de threads e o nome do arquivo foram passados na linha de comando
   if(argc<3) {
      printf("Digite: %s <numero de threads>  <nome do arquivo>\n", argv[0]); return 1;
   }
   //numero de threads
   nthreads = atoi(argv[1]);
   
  //inicia o semaforo binario
  sem_init(&mutexCons, 0, 1);
  sem_init(&slotCheio, 0, 0);
  sem_init(&slotVazio, 0, QTD_LINHAS);


  for(t=0; t<nthreads; t++) {
    //aloca espaco para o identificar da thread
    if ((id = malloc(sizeof(int))) == NULL) {
       pthread_exit(NULL); return 1;
    }

    *id=t;
    //printf("--Cria a thread %d\n", t);
    if (pthread_create(&tid[t], NULL, consumidor, (void *)id)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }
  
  //--espera todas as threads terminarem
  for (t=0; t<nthreads; t++) 
    if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    }
   
  produtor(argv[2]);
  pthread_exit(NULL);
}

