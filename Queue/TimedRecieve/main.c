#include <mqueue.h>
#include <fcntl.h>            
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define ERROR(str){ printf("ERROR: %s/n", str); exit(1); }


static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-t wait_time] [-n] mq-name\n", progName);
    fprintf(stderr, "       -t wait_time Set wait time (seconds) for receiving\n");
    fprintf(stderr, "       -n           Use O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    ssize_t numRead;

    unsigned int time
    int flags = O_RDONLY;
    
    int opt;
    while ((opt = getopt(argc, argv, "nt:")) != -1) 
    {
        switch (opt) 
        {
            case 'n':   
                flags |= O_NONBLOCK;        
                break;
            case 't':
                time = atoi(optarg);
                break;
            default:    
                usageError(argv[0]);
        }
    }

    if (optind >= argc)
        usageError(argv[0]);

    mqd_t mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1)
        ERROR("mq_open");

    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1)
        ERROR("mq_getattr");

    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        ERROR("malloc");

    unsigned int prio;
    numRead = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
    if (numRead == -1)
        ERROR("mq_receive");

    printf("Read %ld bytes; priority = %u\n", (long) numRead, prio);
    if (write(STDOUT_FILENO, buffer, numRead) == -1)
        ERROR("write");
    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}