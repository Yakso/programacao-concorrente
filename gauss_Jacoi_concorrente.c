/* Paulo Yamagishi DRE:121072893 */
/* Gustavo Mariz DRE:121073784 */
/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Trabalho Final*/
/* Codigo: Implementação do algoritmo de Gauss-Jacobi usando programação concorrente. */

// Bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h> 
#include <math.h>

// Variáveis globais
double *new_values, *A, *b, *old_values, epsilon; // Vetor que armazena os valores novos
int N, nthreads; // Variáveis para controle de dimensão
int it = 0; // Variável para contagem de iterações
int conv = 1; //Variável que define quando convergiu
pthread_mutex_t mutex; // Variável para controle de exclusão mútua
pthread_cond_t cond; // Variável para controle de sincronização


// Função que gera a matriz A e o vetor b do sistema linear Ax = b (A é uma matriz quadrada diagonal dominante) (b é um vetor aleatório)
void gera_Ab(){
    double sum;
    srand(1);

    for(int i = 0; i < N; i++){
        sum = 0;
        for(int j = 0; j < N; j++){
            if(i != j){
                A[i*N + j] = (double)rand()/(double)rand();
                sum += A[i*N + j];
            }
        }
        A[i*N + i] = sum * 5; // Garante a diagonal dominante
        b[i] = (double)rand()/(double)rand(); // Randomiza o vetor b    
    }
}

// Função que imprime a matriz A e o vetor b
void imprime_Ab(){
    int i, j;

    printf("Matriz A:\n");
    for(i = 0; i < N; i++){
        for(j = 0; j < N; j++){
            printf("%.2f ", A[i*N + j]);
        }
        printf("\n");
    }
    printf("\nVetor b:\n");
    for(i = 0; i < N; i++){
        printf("%.2f ", b[i]);
    }
    printf("\n");
}

//Função que inicia os vetores de valores antigos e novos
void inicia_vetores(){
    int i;

    for(i = 0; i < N; i++){
        new_values[i] = 1;  
        old_values[i] = 0;  
    }
}

// Função que multiplica a matriz A por um vetor (Ax = b)
double *multiplica_matriz_vetor(double *A, double *x){
    int i, j;
    double soma = 0;
    double *result = (double *) malloc(sizeof(double)*N);

    for(i = 0; i < N; i++){
        soma = 0;
        for(j = 0; j < N; j++){
            soma += A[i*N + j] * x[j];
        }
        result[i] = soma;
    }

    return result;
}

// Função que calcula a distância entre dois vetores
double distancia(double *x, double *y){
    double soma = 0;
    int i;

    for(i = 0; i < N; i++){
        soma += (x[i] - y[i])*(x[i] - y[i]);
    }
    return (double)sqrt(soma);
}

// Função que contém o corpo do while (implementação bruta do Gauss-Jacobi)
void gauss_jacobi_calculo() {
    // Atualiza os valores antigos
    double soma = 0;
    // Calcula os novos valores
    for(int i = 0; i < N; i++){
        old_values[i] = new_values[i];
    }
    for(int i = 0; i < N; i++){
        soma = 0;
        for(int j = 0; j < N; j++){
            if(i != j){
                soma += A[i*N + j] * old_values[j];
            }
        }
        new_values[i] = (b[i] - soma) / A[i*N + i];
    }
}

// Gauss-Jacobi sequencial
void gaussJacobi_Sequencial(int version){
    int i, j;
    int iter = 0;

    // Inicializa os vetores de valores antigos e novos
    inicia_vetores();

    if (version == 1){
        // Enquanto não convergir
        while(distancia(new_values, old_values) > epsilon){
            iter++;
            gauss_jacobi_calculo();
        }
    }
    else {
        // Enquanto não convergir
        while(distancia(multiplica_matriz_vetor(A,new_values), b) > epsilon){
            iter++;
            gauss_jacobi_calculo();
        }
    }

    // Imprime o número de iterações
    printf("\n---------------------------------------------------------\n");
    printf("Iterações: %d\n", iter);
}

// Função barreira para sincronização das threads
void barreira(int nthreads){
    static int count = 0;
    // Soma as iterações de cada thread
    pthread_mutex_lock(&mutex);
    it += 1;
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&mutex);
    count++;
    if(count < nthreads){
        pthread_cond_wait(&cond, &mutex);
    }else{
        count = 0;
        if (distancia(new_values, old_values) < epsilon) {conv = 0;}
        pthread_cond_broadcast(&cond);
    }
    pthread_mutex_unlock(&mutex);
}

// Função que imprime os resultados 
void imprime_resultados_sequenciais(int version){
    // Variáveis para controle de tempo
    struct timespec start, end;
    // Inicializa as variáveis de controle de tempo
    double elapsed_time_seq;
    // vetor auxiliar para armazenar o resultado da multiplicação de A por new_values
    double *aux = (double *) malloc(sizeof(double)*N);

    // Executa o algoritmo sequencial com a versão passada no parâmetro
    clock_gettime(CLOCK_MONOTONIC, &start);
    gaussJacobi_Sequencial(version);
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time_seq = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Tempo sequencial versão %d: %.25f\n", version, elapsed_time_seq);
    aux = multiplica_matriz_vetor(A, new_values);
    printf("Distancia: %.25f\n", distancia(aux, b));
    printf("---------------------------------------------------------\n\n");
    
}

// Função Gauss-Jacobi concorrente
void *gaussJacobi_Concorrente(void *arg){
    int inicio = *((int *) arg);
    int iter = 0;
    double soma = 0;

    // Enquanto não convergir
    while(conv){
        iter++;
        // Atualiza os valores antigos
        for(int i = inicio; i < N; i += nthreads){
            old_values[i] = new_values[i];
        }
        // Atualiza os valores novos
        for(int i = inicio; i < N; i += nthreads){
            soma = 0;
            for(int j = 0; j < N; j++){
                if(i != j){
                    soma += A[i*N + j] * old_values[j];
                }
            }
            new_values[i] = (b[i] - soma) / A[i*N + i];
        }
        barreira(nthreads);
    }
    pthread_exit(NULL);
}

// Função principal
int main(int argc, char *argv[]) {
    // Variáveis para controle de tempo
    struct timespec start, end;
    double elapsed_time_seq;
    // Inicializa as variáveis de controle de tempo
    int i, j, k;
    // Variáveis para controle de threads
    pthread_t *tid;

    // Inicializa as variáveis de controle de dimensão
    N = atoi(argv[1]);
    nthreads = atoi(argv[2]);
    epsilon = atof(argv[3]);

    // Inicializa as variáveis de controle de matrizes e vetores
    A = (double *) malloc(sizeof(double)*N*N);
    b = (double *) malloc(sizeof(double)*N);
    new_values = (double *) malloc(sizeof(double)*N);
    old_values = (double *) malloc(sizeof(double)*N);


    // Inicializa as variáveis de controle de threads
    tid = (pthread_t *) malloc(sizeof(pthread_t)*nthreads);

    // Gera a matriz A e o vetor b do sistema linear Ax = b
    gera_Ab();
    imprime_Ab();

    // Executa o algoritmo sequencial 
    imprime_resultados_sequenciais(1);
    double *vet1 = (double *) malloc(sizeof(double)*N);
    for (i = 0; i < N; i++) {
        vet1[i] = new_values[i];
    }
    inicia_vetores();
    imprime_resultados_sequenciais(2);

    // Executa o algoritmo concorrente
    inicia_vetores();

    clock_gettime(CLOCK_MONOTONIC, &start);
    // Cria as threads
    for (i = 0; i < nthreads; i++) {
        int *arg = malloc(sizeof(int)); *arg = i;
        if (pthread_create(tid + i, NULL, gaussJacobi_Concorrente, arg)) {
            printf("--ERRO: pthread_create()\n"); exit(-1);
        }
    }
    // Espera as threads terminarem
    for (i = 0; i < nthreads; i++) {
        if (pthread_join(*(tid+i), NULL)) {
            printf("--ERRO: pthread_join()\n"); exit(-1);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed_time_seq = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    double *vet2 = (double *) malloc(sizeof(double)*N);
    for (i = 0; i < N; i++) {
       vet2[i] = new_values[i];
    }

    printf("\n---------------------------------------------------------\n");
    printf("Iterações: %d\n", it/nthreads);
    printf("Tempo concorrente: %.25f\n", elapsed_time_seq);
    printf("Distancia: %.25f\n", distancia(multiplica_matriz_vetor(A, new_values), b));
    printf("---------------------------------------------------------\n\n");

    printf("Distancia entre os vetores: %.25f\n", distancia(vet1, vet2));
    //Libera a memória alocada
    free(A);
    free(b);
    free(new_values);
    free(tid);

    return 0;
}
