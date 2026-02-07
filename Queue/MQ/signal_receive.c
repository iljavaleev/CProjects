#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>


#define ERROR(str){ printf("%s\n", str); exit(1); }
#define NOTIFY_SIG SIGUSR1

static void handler(int sig)
{
    printf("Signal cought\n");
    /* Просто прерываем вызов sigsuspend() */
}

int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        ERROR("prog mq-name\n");

    mqd_t mqd = mq_open(argv[1], O_RDONLY | O_NONBLOCK);
    if (mqd == (mqd_t) -1)
        ERROR("mq_open");
    
    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1)
        ERROR("mq_getattr");

    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        ERROR("malloc");
    
    sigset_t blockMask, emptyMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask, NOTIFY_SIG);
    /*
        изменить список блокированных в данный момент сигналов
        SIG_BLOCK The set of blocked signals is the union of the current set and the set argument
    */

    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1)
        ERROR("sigprocmask");
    
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(NOTIFY_SIG, &sa, NULL) == -1)
        ERROR("sigaction");

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    if (mq_notify(mqd, &sev) == -1)
        ERROR("mq_notify");
        
    sigemptyset(&emptyMask);
    int numRead;
    for (;;) 
    {
        /*
            sigsuspend() temporarily replaces the signal mask of the calling
            thread with the mask given by mask and then suspends the thread
            until delivery of a signal whose action is to invoke a signal
            handler or to terminate a process. 
        */
        
        sigsuspend(&emptyMask); // Все сигналы, при получении сообщения == сигнала подписка заканчивается
        if (mq_notify(mqd, &sev) == -1) // возобновляем подписку
            ERROR("mq_notify again");
        while ((numRead = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) >= 0) // читаем из очереди
            printf("Read %ld bytes\n", (long) numRead);
        if (errno != EAGAIN) /* Непредвиденная ошибка */
            ERROR("mq_receive");
    }
     
    exit(EXIT_SUCCESS);
}

