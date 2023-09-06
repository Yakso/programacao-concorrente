/* Achando primos */
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<math.h>
#include "timer.h"

long long int *vet; //vetor para trabalhar os elementos
int nprimos = 0; //numero de primos encontrados
int nthreads; //numero de threads
pthread_mutex_t mutex; //variavel de lock para exclusao mutua


typedef struct{
   int id; //identificador do elemento que a thread ira processar
   int N; //Nensao das estruturas de entrada
} tArgs;


//funcao que verifica se um numero eh primo ou nao
int ehPrimo(long long int n) {
   int i;
   if (n<=1) return 0;
   if (n==2) return 1;
   if (n%2==0) return 0;
   for(i=3; i<sqrt(n)+1; i+=2)
      if(n%i==0) return 0;
   return 1;
}


//funcao que as threads executarao
void * tarefa(void *arg) {
   int nprimos_local = 0;
   tArgs *args = (tArgs*) arg;
   //printf("Thread %d\n", args->id);
   for(int i=args->id; i<args->N; i+=nthreads)
      if(ehPrimo(vet[i])) nprimos_local++;

   pthread_mutex_lock(&mutex); // inicio da secao critica
   nprimos += nprimos_local;
   pthread_mutex_unlock(&mutex); // fim da secao critica

   pthread_exit(NULL);
}


//fluxo principal
int main(int argc, char* argv[]) {
   long long int N; // numero de elementos do vetor
   pthread_t *tid; //identificadores das threads no sistema
   tArgs *args; //identificadores locais das threads e Nensao
   double inicio, fim, delta;
   
   GET_TIME(inicio);
   //leitura e avaliacao dos parametros de entrada
   if(argc<3) {
      printf("Digite: %s <numero de elementos> <numero de threads>\n", argv[0]);
      return 1;
   }
   N = atoll(argv[1]);
   nthreads = atoi(argv[2]);
   if (nthreads > N) nthreads=N;

   //alocacao de memoria para as estruturas de dados
   vet = (long long int *) malloc(sizeof(long long int) * N);
   if (vet == NULL) {printf("ERRO--malloc\n"); return 2;}

   //inicializacao das estruturas de dados de entrada e saida
   for(int i=0; i<N; i++) vet[i] = i+1; 

   GET_TIME(fim);
   delta = fim - inicio;
   printf("----------------------------------------------\n");
   printf("|**Tempo inicializacao**:%lf\n", delta);
   printf("----------------------------------------------\n");

   // checando se os elementos sao primos
   GET_TIME(inicio);
   //alocacao das estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}
   args = (tArgs*) malloc(sizeof(tArgs)*nthreads);
   if(args==NULL) {puts("ERRO--malloc"); return 2;}
   //criacao das threads
   for(int i=0; i<nthreads; i++) {
      (args+i)->id = i;
      (args+i)->N = N;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))){
         puts("ERRO--pthread_create"); return 3;
      }
   } 
   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("|**Tempo para encontrar primos (quantidade de elementos**: %lld) (nthreads %d): %lf\n", N, nthreads, delta);
   printf("----------------------------------------------\n");
   printf("|Quantidade de primos: %d\n", nprimos);
   printf("----------------------------------------------\n");

   //liberacao da memoria
   GET_TIME(inicio);
   free(vet);
   free(args);
   free(tid);
   GET_TIME(fim)   
   delta = fim - inicio;
   printf("|**Tempo finalizacao**:%lf\n", delta);
   printf("----------------------------------------------\n");

   return 0;
}
