#include <pthread.h>

// Usando variáveis de condição pede a exclusão mútua para leitura e escrita
void InicioLeitura(int* contador_leitores, int* contador_escritores, pthread_mutex_t* mutex, pthread_cond_t* cond_leit);

// Finaliza a leitura
void FimLeitura(int* contador_leitores, pthread_mutex_t* mutex, pthread_cond_t* cond_escr);

// Inicia a escrita
void InicioEscrita(int* contador_escritores, int* contador_leitores, pthread_mutex_t* mutex, pthread_cond_t* cond_escr);

// Finaliza a escrita
void FimEscrita(int* contador_escritores, pthread_mutex_t* mutex, pthread_cond_t* cond_escr, pthread_cond_t* cond_leit);