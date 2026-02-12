#include <signal.h>
#include <mqueue.h>
#include <fcntl.h>             
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <stdatomic.h>

#define ERROR(str){ printf("ERROR: %s\n", str); exit(1); }

#define NOTIFY_SIG SIGUSR1

atomic_bool not_sent = 0;

static void not_handler(int sig)
{
    printf("notification has been sended; read queue\n");
    not_sent = 1;
}

static void alrm_handler(int sig)
{
    if (!not_sent)
    {
        printf("after 10s:\n");
        return;
    }
    not_sent = 0;
}

int main(int argc, char *argv[])
{

    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        ERROR("progname mq-name\n");

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
    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1)
        ERROR("sigprocmask");
    
    struct sigaction sa_not;
    sigemptyset(&sa_not.sa_mask);
    sa_not.sa_flags = 0;
    sa_not.sa_handler = not_handler;
    if (sigaction(NOTIFY_SIG, &sa_not, NULL) == -1) ERROR("sigaction");

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    if (mq_notify(mqd, &sev) == -1)
        ERROR("mq_notify");

    struct sigaction sa_al;
    sigemptyset(&sa_al.sa_mask);
    sa_al.sa_flags = 0;
    sa_al.sa_handler = alrm_handler;
    if (sigaction(SIGALRM, &sa_al, NULL) == -1) ERROR("sigaction");

    ssize_t numRead;
    sigemptyset(&emptyMask);

    for (;;) 
    {
        alarm(10);
        sigsuspend(&emptyMask);
               
        while ((numRead = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) > 0)
        {   
            printf("Message: ");
            printf("%s\n", (char*) buffer);
        }

        if (errno != EAGAIN)            
            ERROR("mq_receive");
    }
}
