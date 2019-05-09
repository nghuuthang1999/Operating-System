#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define NUM_THREAD 8
size_t numOfPointsInCir = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Coordinate{
    double x;
    double y;
};

bool isInsideCircle(struct Coordinate point, struct Coordinate center, double radius){
    double d = sqrt(  (point.x - center.x)*(point.x - center.x) + (point.y - center.y)*(point.y - center.y)  );
    if (d < radius)
        return true;
    else
        return false;
}

void* ranCoordinate(void* arg){ 
    size_t nPoint = (size_t)arg;
    int seed = rand();
    struct Coordinate center = {.x = 0.0, .y = 0.0};
    double radius = 1.0;
    for (size_t i = 0; i < nPoint; ++i){
        struct Coordinate point;
        point.x = (rand_r(&seed) % 10000) / 10000.0;
        point.y = (rand_r(&seed) % 10000) / 10000.0;
        if (isInsideCircle(point, center, radius)){
            pthread_mutex_lock(&mutex);
            ++numOfPointsInCir;
            pthread_mutex_unlock(&mutex);
        }
        else
            continue;
    }
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    assert(argc == 2);
    size_t N = atoi(argv[1]);
    pthread_t thread[NUM_THREAD];
    size_t nPoint = N / NUM_THREAD;
    time_t start, end;
    srand(time(NULL));
    time(&start);
    for (int i = 0; i < NUM_THREAD; ++i){ 
        pthread_create(&thread[i], NULL, ranCoordinate, (void*)nPoint);
    }
    for (int i = 0; i < NUM_THREAD; ++i){
        pthread_join(thread[i], NULL);
    }
    double pi = (4 * numOfPointsInCir) / (double)N;
    time(&end);
    printf("pi = %lf\n", pi);
    double duration = difftime(end, start);
    printf("Exe time = %.3lfs\n", duration);
    pthread_exit(NULL);
    return 0;
}
