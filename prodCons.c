#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define TAM 5
#define TOTAL 20

typedef struct {
    int almacen[TAM];
    int entrada;
    int salida;

    sem_t vacios;
    sem_t llenos;
    sem_t mutex;

} Datos;

void* productor(void* arg);
void* consumidor(void* arg);

int main() {
    pthread_t hiloP, hiloC;
    Datos datos = {
        .entrada = 0,
        .salida = 0
    };

    sem_init(&datos.vacios, 0, TAM);
    sem_init(&datos.llenos, 0, 0);
    sem_init(&datos.mutex, 0, 1);
    pthread_create(&hiloP, NULL, productor, &datos);
    pthread_create(&hiloC, NULL, consumidor, &datos);
    pthread_join(hiloP, NULL);
    pthread_join(hiloC, NULL);
    sem_destroy(&datos.vacios);
    sem_destroy(&datos.llenos);
    sem_destroy(&datos.mutex);
    return 0;
}

void* productor(void* arg) {
    Datos* datos = (Datos*)arg;
    for (int i = 1; i <= TOTAL; i++) {
        sem_wait(&datos->vacios);
        sem_wait(&datos->mutex);
        datos->almacen[datos->entrada] = i;
        printf("Productor: produjo %d\n", i);
        datos->entrada = (datos->entrada + 1) % TAM;
        sem_post(&datos->mutex);
        sem_post(&datos->llenos);
        sleep(1);
    }

    return NULL;
}

void* consumidor(void* arg) {
    Datos* datos = (Datos*)arg;
    for (int i = 1; i <= TOTAL; i++) {
        sem_wait(&datos->llenos);
        sem_wait(&datos->mutex);
        int elemento = datos->almacen[datos->salida];
        printf("Consumidor: consumio %d\n", elemento);
        datos->salida = (datos->salida + 1) % TAM;
        sem_post(&datos->mutex);
        sem_post(&datos->vacios);
        sleep(2);
    }

    return NULL;
}