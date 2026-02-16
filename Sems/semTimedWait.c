#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define ERROR(str){ printf("Error %s\n", str); exit(1); }

static sem_t sem;

static void* threadFunc(void *arg)
{
    /* wait for random time from 1 to 10 second before increment sem */
    srand(time(NULL));
    sleep((rand() % 10) + 1);
    
    if (sem_post(&sem) == -1)
       ERROR("sem_post");
    
    return NULL;
}

int main(int argc, char *argv[])
{
    struct timespec timer;
    pthread_t t;

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        ERROR("set seconds to wait\n");
    
    /* init sem with 0 */
    if (sem_init(&sem, 0, 0) == -1)
        ERROR("sem_init");
    
    if (pthread_create(&t, NULL, threadFunc, NULL) != 0)
        ERROR("create thread");

    if (clock_gettime(CLOCK_REALTIME, &timer) == -1) 
        ERROR("clock_gettime");
    
    timer.tv_sec += atoi(argv[1]);
    if (sem_timedwait(&sem, &timer) == -1)
    {
        if (errno == ETIMEDOUT)
        {
            printf("time is up\n");
            exit(EXIT_SUCCESS);
        }
        else
            ERROR("sem_wait");
    }
    if (pthread_join(t, NULL) != 0)
        ERROR("hread join");
    
    printf("%ld sem_wait() succeeded\n", (long) getpid());
    exit(EXIT_SUCCESS);
}

