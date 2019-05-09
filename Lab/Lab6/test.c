#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
static volatile int counter = 0;
pthread_mutex_t mutex;
static sem_t semaphone;

void* mythread(void *arg){
    printf("%s : begin\n" , (char *) arg );
    sem_init(&semaphone, 0, 1);
    for (int i = 0; i < 1e7; i++) {
    counter = counter + 1;
    }
    sem_destroy(&semaphone);
    printf("%s : done\n" , (char *) arg );
    return NULL;
}
int main(int argc , char *argv[]){   
    pthread_t p1, p2;
    printf("main: begin (counter = %d)\n" , counter );
    pthread_create(&p1, NULL, mythread, "A");
    pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    pthread_join(p1, NULL);
    pthread_join(p2, NULL);
    printf("main: done with both (counter = %d)\n" , counter );
    return 0;
}