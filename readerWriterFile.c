#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

#define QUEUE_SIZE 5  

char queue[QUEUE_SIZE];
int front = 0, rear = 0;

sem_t empty;  
sem_t full;
pthread_mutex_t mutex;

void enqueue(char item) {
    queue[rear] = item;
    rear = (rear + 1) % QUEUE_SIZE;
}

char dequeue() {
    char item = queue[front];
    front = (front + 1) % QUEUE_SIZE;
    return item;
}

void *producer(void *arg) {
    FILE *fp = fopen("string.txt", "r");
    if (!fp) {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    char ch;
    while ((ch = fgetc(fp)) != EOF) {
        sem_wait(&empty);             
        pthread_mutex_lock(&mutex);  

        enqueue(ch);

        pthread_mutex_unlock(&mutex);  
        sem_post(&full);         
    }

    sem_wait(&empty);
    pthread_mutex_lock(&mutex);
    enqueue('\0'); 
    pthread_mutex_unlock(&mutex);
    sem_post(&full);

    fclose(fp);
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    char ch;
    while (1) {
        sem_wait(&full);             
        pthread_mutex_lock(&mutex);     
        
        ch = dequeue();

        pthread_mutex_unlock(&mutex);   
        sem_post(&empty);               

        if (ch == '\0') break;

        printf("%c", ch);
        fflush(stdout);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t prod_thread, cons_thread;

    sem_init(&empty, 0, QUEUE_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    printf("\n");
    return 0;
}
