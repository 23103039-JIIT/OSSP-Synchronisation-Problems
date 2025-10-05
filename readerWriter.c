#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_READERS 5
#define NUM_WRITERS 3

int data = 0;
int rc = 0;

sem_t w; 
sem_t mutex; 
sem_t queue;

void msleep(long ms) {
    usleep(ms * 1000);
}

void *reader(void *arg) {
    int id = *(int *)arg;
    free(arg);

    for (int i = 0; i < 5; ++i) {
        sem_wait(&queue); 
        sem_wait(&mutex);
        rc++;
        if (rc == 1) sem_wait(&w);
        sem_post(&mutex);
        sem_post(&queue);   

        printf("[Reader %d] starts reading. shared_data = %d\n", id, data);
        msleep(50 + rand() % 150);  
        printf("[Reader %d] finished reading.\n", id);

        sem_wait(&mutex);
        rc--;
        if (rc == 0) sem_post(&w);
        sem_post(&mutex);

        msleep(100 + rand() % 400);
    }

    return NULL;
}

void *writer(void *arg) {
    int id = *(int *)arg;
    free(arg);

    for (int i = 0; i < 5; ++i) {
        sem_wait(&queue);
        sem_wait(&w);   
        sem_post(&queue);

        printf("    [Writer %d] starts writing.\n", id);
        int new_value = data + 1;
        msleep(100 + rand() % 200); 
        data = new_value;
        printf("    [Writer %d] finished writing. shared_data -> %d\n", id, data);

        sem_post(&w);

        msleep(200 + rand() % 500);
    }

    return NULL;
}

int main(void) {
    srand((unsigned)time(NULL));

    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];

    sem_init(&w, 0, 1);
    sem_init(&mutex, 0, 1);
    sem_init(&queue, 0, 1);

    for (int i = 0; i < NUM_READERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&readers[i], NULL, reader, id);
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&writers[i], NULL, writer, id);
    }

    for (int i = 0; i < NUM_READERS; ++i) pthread_join(readers[i], NULL);
    for (int i = 0; i < NUM_WRITERS; ++i) pthread_join(writers[i], NULL);

    sem_destroy(&w);
    sem_destroy(&mutex);
    sem_destroy(&queue);

    printf("All threads finished. Final shared_data = %d\n", data);
    return 0;
}
