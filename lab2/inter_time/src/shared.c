#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define BUF_SIZE 10000
#define N_ROUNDS 1000000

char  shared[BUF_SIZE];
char  copy[BUF_SIZE];
int   has_ping = 0;
int   has_pong = 0;
pthread_mutex_t   mtx       = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t    cond_ping = PTHREAD_COND_INITIALIZER;
pthread_cond_t    cond_pong = PTHREAD_COND_INITIALIZER;

// разница во времени в наносекундах
long diff_nsec(const struct timespec *start, const struct timespec *end) {
    return (end->tv_sec  - start->tv_sec) * 1000000000L
         + (end->tv_nsec - start->tv_nsec);
}

void *thread_A(void *arg __attribute__((unused))) {
    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    for (int i = 0; i < N_ROUNDS; i++) {
        // отправляем ping
        pthread_mutex_lock(&mtx);
        memcpy(shared, copy, BUF_SIZE);
        has_ping = 1;
        pthread_cond_signal(&cond_ping);

        // ждем pong
        while (!has_pong)
            pthread_cond_wait(&cond_pong, &mtx);

        // memccpy(bufA, bufB, BUF_SIZE, sizeof(char));

        // printf("[A] received: %s\n", shared);
        has_pong = 0;
        pthread_mutex_unlock(&mtx);
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    long elapsed = diff_nsec(&t0, &t1);
    printf("%d rounds in %.3f ms\n",
           N_ROUNDS, elapsed / 1e6);

    printf("for itteration = %.3f us\n", elapsed / 1e3 / 2 / N_ROUNDS);

    return NULL;
}

void *thread_B(void *arg __attribute__((unused))) {
    for (int i = 0; i < N_ROUNDS; i++) {
        // ждем ping
        pthread_mutex_lock(&mtx);
        while (!has_ping)
            pthread_cond_wait(&cond_ping, &mtx);

        // memccpy(bufB, bufA, BUF_SIZE, sizeof(char));
        // printf("[B] received: %s\n", shared);
        has_ping = 0;

        // отправляем pong
        memcpy(shared, copy, BUF_SIZE);
        has_pong = 1;
        pthread_cond_signal(&cond_pong);
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main(void) {
    for (int i = 0; i < BUF_SIZE - 1; ++i) {
        copy[i] = 'a';
    }
    copy[BUF_SIZE - 1] = '\0';

    pthread_t A, B;
    pthread_create(&A, NULL, thread_A, NULL);
    pthread_create(&B, NULL, thread_B, NULL);
    pthread_join(A, NULL);
    pthread_join(B, NULL);
    return 0;
}
