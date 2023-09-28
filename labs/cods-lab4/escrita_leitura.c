#include <pthread.h>
#include "escritura_leitura.h"

void InicioLeitura(int* contador_leitores, int* contador_escritores, pthread_mutex_t* mutex, pthread_cond_t* cond_leit) {
    pthread_mutex_lock(mutex);
    while (*contador_escritores > 0) {
        pthread_cond_wait(cond_leit, mutex);
    }
    (*contador_leitores)++;
    pthread_mutex_unlock(mutex);
}

void FimLeitura(int* contador_leitores, pthread_mutex_t* mutex, pthread_cond_t* cond_escr) {
    pthread_mutex_lock(mutex);
    (*contador_leitores)--;
    if (*contador_leitores == 0) {
        pthread_cond_signal(cond_escr);
    }
    pthread_mutex_unlock(mutex);
}

void InicioEscrita(int* contador_leitores, int* contador_escritores, pthread_mutex_t* mutex, pthread_cond_t* cond_escr) {
    pthread_mutex_lock(mutex);
    while (*contador_leitores > 0 || *contador_escritores > 0) {
        pthread_cond_wait(cond_escr, mutex);
    }
    (*contador_escritores)++;
    pthread_mutex_unlock(mutex);
}

void FimEscrita(int* contador_escritores, pthread_mutex_t* mutex, pthread_cond_t* cond_escr, pthread_cond_t* cond_leit) {
    pthread_mutex_lock(mutex);
    (*contador_escritores)--;
    pthread_cond_signal(cond_escr);
    pthread_cond_broadcast(cond_leit);
    pthread_mutex_unlock(mutex);
}