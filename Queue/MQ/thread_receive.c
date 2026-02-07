#include <mqueue.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>


#define ERROR(str){ printf("ERROR %s\n", str); exit(1); }


static void notifySetup(mqd_t *mqdp);


static void threadFunc(union sigval sv)
{
    mqd_t *mqdp = sv.sival_ptr;
    struct mq_attr attr;
    if (mq_getattr(*mqdp, &attr) == -1)
        ERROR("mq_getattr");
    
    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        ERROR("malloc");

    notifySetup(mqdp);
    int numRead;
    while ((numRead = mq_receive(*mqdp, buffer, attr.mq_msgsize, NULL)) >= 0)
        printf("Read %ld bytes\n", (long) numRead);
    
    if (errno != EAGAIN) 
        ERROR("mq_receive");
    free(buffer);

    pthread_exit(NULL);
}


static void notifySetup(mqd_t *mqdp)
{
    struct sigevent sev;
    sev.sigev_notify = SIGEV_THREAD; 
    sev.sigev_notify_function = threadFunc;
    sev.sigev_notify_attributes = NULL;
    sev.sigev_value.sival_ptr = mqdp; 
    if (mq_notify(*mqdp, &sev) == -1)
        ERROR("mq_notify"); 
}


int main(int argc, char **argv)
{
    mqd_t mqd;
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        ERROR("prog mq-name\n");
    mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1)
        ERROR("mq_open");
    notifySetup(&mqd);
    pause();
    exit(EXIT_SUCCESS);
}


