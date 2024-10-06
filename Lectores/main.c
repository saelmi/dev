#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t readers_ok = PTHREAD_COND_INITIALIZER;
pthread_cond_t writers_ok = PTHREAD_COND_INITIALIZER;
int readers = 0;  // Número de lectores activos
int writers = 0;  // Número de escritores activos

void* reader(void* arg) {
    pthread_mutex_lock(&lock);
    while (writers > 0) {
        pthread_cond_wait(&readers_ok, &lock);
    }
    readers++;
    pthread_mutex_unlock(&lock);

    // Leer el recurso compartido
    printf("Lector %ld está leyendo.\n", (long)arg);
    sleep(1);  // Simula el tiempo de lectura

    pthread_mutex_lock(&lock);
    readers--;
    if (readers == 0) {
        pthread_cond_signal(&writers_ok);
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

void* writer(void* arg) {
    pthread_mutex_lock(&lock);
    while (readers > 0 || writers > 0) {
        pthread_cond_wait(&writers_ok, &lock);
    }
    writers++;

    // Escribir en el recurso compartido
    printf("Escritor %ld está escribiendo.\n", (long)arg);
    sleep(1);  // Simula el tiempo de escritura

    writers--;
    if (writers == 0) {
        pthread_cond_broadcast(&readers_ok);
    } else {
        pthread_cond_signal(&writers_ok);
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

int main() {
    pthread_t r1, r2, w1;
    pthread_create(&r1, NULL, reader, (void*)1);
    pthread_create(&r2, NULL, reader, (void*)2);
    pthread_create(&w1, NULL, writer, (void*)1);

    pthread_join(r1, NULL);
    pthread_join(r2, NULL);
    pthread_join(w1, NULL);

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&readers_ok);
    pthread_cond_destroy(&writers_ok);
    return 0;
}
