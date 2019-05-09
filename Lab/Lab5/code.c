#include <stdio.h> // ? ahihi
// tao ô chat bị sao á
// ko chat đc
// chat terminal nhục quá
// hay do
#include <pthread.h> // thấy :v
//real-time mà :v//hahaahahhaa
void* hello(void *tid) {
    printf("Hello from thread %d\n" , (int)tid );
}
int main() {
    pthread_t tid [10];//ahihi đồ ngốc
    for (int i = 0; i < 10; i++) {
        pthread_create(&tid[i] , NULL, hello, (void*)i);
        pthread_join(tid[i], NULL);
    }
    pthread_exit(NULL);
}