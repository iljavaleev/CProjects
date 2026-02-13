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

    sigset_t blockMask;
    sigemptyset(&blockMask);
    sigaddset(&blockMask, NOTIFY_SIG);
    sigaddset(&blockMask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1)
        ERROR("sigprocmask");
    
    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    sev.sigev_value.sival_int = mqd;
    if (mq_notify(mqd, &sev) == -1)
        ERROR("mq_notify");

    ssize_t numRead;
    siginfo_t info;
    int sigNum;

    for (;;) 
    {
        alarm(10);
        if ((sigNum = sigwaitinfo(&blockMask, &info)) == -1)
            ERROR("sigprocmask");
        
        switch (sigNum) 
        {
            case NOTIFY_SIG:
                printf("Received signal notification for MQ %d\n", info.si_value);
                break;
            case SIGALRM:
                printf("Received SIGALRM after 10 seconds\n");
                break;
            default:
                printf("Received unexpected signal %d.\n", sigNum);
        }

        while ((numRead = mq_receive(mqd, buffer, attr.mq_msgsize, NULL)) > 0)
        {   
            printf("Message: ");
            printf("%s\n", (char*) buffer);
        }

        if (errno != EAGAIN)            
            ERROR("mq_receive");
    }

    exit(EXIT_SUCCESS);
}
