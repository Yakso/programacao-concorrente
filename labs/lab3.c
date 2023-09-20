/* Testando a barreira*/
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//variaveis globais
int aux = 0;
int nthreads; //numero de threads
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;


void barreira(int nthreads) {
   pthread_mutex_lock(&x_mutex); //inicio secao critica
   if (aux < nthreads - 1) { 
      aux++;
      pthread_cond_wait(&x_cond, &x_mutex);
   }
   else { 
      aux = 0;
      pthread_cond_broadcast(&x_cond);
   }
   pthread_mutex_unlock(&x_mutex); //fim secao critica
}

//funcao que as threads executarao
void * tarefa(void *arg) {
   int id = *(int*)arg; // identificador da thread
     
   printf("Olá da thread %d\n", id+1);
   barreira(nthreads); // espera todas as threads executarem o printf acima
   printf("Que dia bonito %d\n", id+1);
   barreira(nthreads); // espera todas as threads executarem o printf acima
   printf("Até breve da thread %d\n", id+1);

   pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char* argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   int *t; //identificadores locais das threads 

   pthread_mutex_init(&x_mutex, NULL);
   pthread_cond_init (&x_cond, NULL);
  
   //leitura e avaliacao dos parametros de entrada
   if(argc<2) {
      printf("Digite: <numero de threads>\n");
      return 1;
   }
   nthreads = atoi(argv[1]);

   // aloca espaco para os identificadores das threads
   tid = (pthread_t*) malloc(sizeof(pthread_t)*nthreads);
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //criacao das threads
   for(int i=0; i<nthreads; i++) {
      t = malloc(sizeof(int)); if(!t) {puts("ERRO--malloc"); return 2;}
      *t = i;
      if(pthread_create(tid+i, NULL, tarefa, (void*)t)){
         puts("ERRO--pthread_create"); return 3;
      }
   } 
   //espera pelo termino da threads
   for(int i=0; i<nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }

   //liberacao da memoria
   free(tid);

   // Desaloca variaveis e termina 
   pthread_mutex_destroy(&x_mutex);
   pthread_cond_destroy(&x_cond);

   return 0;
}
