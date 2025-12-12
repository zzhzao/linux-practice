
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

int ticket = 100;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *route(void *arg)
{
    char *id = (char *)arg;
    // pthread_mutex_lock(&mutex);
    while (1)
    {
        pthread_mutex_lock(&mutex);
        if (ticket > 0)
        {
            usleep(1000);
            printf("%s sells ticket:%d\n", id, ticket);
            ticket--;
        }
        else
        {
            break;
        }
        pthread_mutex_unlock(&mutex);
    }
    // pthread_mutex_unlock(&mutex);
}

int main(void)
{
    pthread_t t1, t2, t3, t4;
    pthread_create(&t1, NULL, route, (void *)"thread 1");
    pthread_create(&t2, NULL, route, (void *)"thread 2");
    pthread_create(&t3, NULL, route, (void *)"thread 3");
    pthread_create(&t4, NULL, route, (void *)"thread 4");
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
}