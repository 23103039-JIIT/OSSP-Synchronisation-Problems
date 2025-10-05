#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5

pthread_mutex_t forks[NUM_PHILOSOPHERS]; 

void *philosopher(void *num) {
    int id = *(int *)num;

    while (1) {
        printf("Philosopher %d is thinking.\n", id);
        sleep(rand() % 3 + 1);

        printf("Philosopher %d is hungry.\n", id);

        if (id % 2 == 0) {
            pthread_mutex_lock(&forks[id]);
            printf("Philosopher %d picked up LEFT fork %d.\n", id, id);

            pthread_mutex_lock(&forks[(id + 1) % NUM_PHILOSOPHERS]);
            printf("Philosopher %d picked up RIGHT fork %d.\n", id, (id + 1) % NUM_PHILOSOPHERS);
        } else {
            pthread_mutex_lock(&forks[(id + 1) % NUM_PHILOSOPHERS]);
            printf("Philosopher %d picked up RIGHT fork %d.\n", id, (id + 1) % NUM_PHILOSOPHERS);

            pthread_mutex_lock(&forks[id]);
            printf("Philosopher %d picked up LEFT fork %d.\n", id, id);
        }

        printf("Philosopher %d is eating.\n", id);
        sleep(rand() % 2 + 1);

        pthread_mutex_unlock(&forks[id]);
        pthread_mutex_unlock(&forks[(id + 1) % NUM_PHILOSOPHERS]);

        printf("Philosopher %d finished eating and puts down both forks.\n", id);
        sleep(rand() % 2 + 1);
    }
}

int main() {
    pthread_t threads[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];
    srand(time(NULL));

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) pthread_mutex_init(&forks[i], NULL);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }
    
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) pthread_join(threads[i], NULL);

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) pthread_mutex_destroy(&forks[i]);

    return 0;
}
