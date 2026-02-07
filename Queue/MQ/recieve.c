#include <mqueue.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define ERROR(str){ printf("ERROR: %s\n", str); exit(1); }


static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] mq-name\n", progName);
    fprintf(stderr, " -n Use O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int flags = O_RDONLY;
    int opt;
    while ((opt = getopt(argc, argv, "n")) != -1) 
    {
        switch (opt) 
        {
            case 'n': 
                flags |= O_NONBLOCK; 
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
        ERROR("mq_getattr")
    
    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        ERROR("malloc");
    
    unsigned int prio;
    int numRead = mq_receive(mqd, buffer, attr.mq_msgsize, &prio);
    if (numRead == -1)
        ERROR("mq_receive");
    
    printf("Read %ld bytes; priority = %u\n", (long) numRead, prio);
    if (write(STDOUT_FILENO, buffer, numRead) == -1)
        ERROR("write");
    
    write(STDOUT_FILENO, "\n", 1);

    exit(EXIT_SUCCESS);
}

