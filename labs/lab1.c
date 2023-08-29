/* Paulo Yamagishi DRE:121072893 */
/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Módulo 2 - Laboratório: 1 */
/* Codigo: Elevar os 10000 elementos de um vetor ao quadrado. */

// Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> 


// Constantes
#define N 10000 // N igual ao número de elementos do vetor
#define NTHREADS  8 // Total de threads a serem criadas


// Cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
    int begin, jump, array_size;
    float *value;
}t_Args;


// Função que será executada pelas threads
void *ToSquare (void *arg) {
    t_Args *args = (t_Args *) arg;
    int i;
    
    for(i=(args->begin); i<(args->array_size); i=i+(args->jump)) {
    	args->value[i] = (args->value[i])*(args->value[i]); 
    }
    
    free(arg); // Libera a alocação feita na main
    pthread_exit(NULL);
}


// Função que inicializa o vetor
void initArray(float* array) {
    int i;
    
    srand((unsigned)time(NULL));
    for(i=0; i<N; i++){
        array[i] = (float)rand()/(float)rand();
    }
}


// Função que testa os valores 
void testArray(float* array1, float* array2) {
    int i;
    
    for(i=0; i<N; i++){
        if((array1[i]*array1[i]) != array2[i])
            printf("--ERRO: Failed-Test\n");
    }
    printf("SUCCESS!\n");
}


// Thread principal do programa
int main() {
    pthread_t thread_id[NTHREADS]; // Criando um identificador para cada thread
    t_Args *args; // Ponteiro pra um vetor do tipo t_Args (armazena os parâmetros da thread)
    
    int i;
    float arr[N],aux[N];
    
    initArray(arr); // Array inicializado
    for(i=0; i<N; i++) {  // Cópia do array original para teste no final
    	aux[i] = arr[i];

    	//printf("%f\n", arr[i]); // caso queira checar, descomente essa linha
    }
    
    // Criando as 8 threads que elevam cada elemento do vetor ao quadrado
    for(i=0; i<NTHREADS; i++){
        args = malloc(sizeof(t_Args));
        if (args == NULL) {
            printf("--ERRO: malloc()\n"); exit(-1);
        }
        
        args->begin = i; 
        args->jump = NTHREADS;
        args->array_size = N;
        args->value = arr;
         
        if(pthread_create(&thread_id[i], NULL, ToSquare, (void*) args)){
            printf("--ERRO: pthread_create()\n");
        }
    }
    
    /*printf("VETOR 2\n");
    for(i=0; i<N; i++) {
    	printf("%f\n", arr[i]);
    }*/                               // caso queira checar, descomente essas linhas
    
    //--espera todas as threads terminarem
    for (int i=0; i<NTHREADS; i++) {
        if (pthread_join(thread_id[i], NULL)) {
            printf("--ERRO: pthread_join() \n"); exit(-1); 
        } 
    }
    
    testArray(aux, arr);
    
    pthread_exit(NULL);
    return 0;
}
