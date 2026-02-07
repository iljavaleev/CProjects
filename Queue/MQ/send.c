#include <mqueue.h>
#include <fcntl.h> /* Для определения O_NONBLOCK */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ERROR(str){ printf("ERROR: %s\n", str); exit(1); }


static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-n] mq-name msg [prio]\n", progName);
    fprintf(stderr, " -n Use O_NONBLOCK flag\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
    int flags = O_WRONLY;
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

    if (optind + 1 >= argc)
        usageError(argv[0]);

    mqd_t mqd = mq_open(argv[optind], flags);
    if (mqd == (mqd_t) -1)
        ERROR("mq_open");

    unsigned int prio = (argc > optind + 2) ? atoi(argv[optind + 2]) : 0;
    if (mq_send(mqd, argv[optind + 1], strlen(argv[optind + 1]), prio) == -1)
        ERROR("mq_send");
    
    
    exit(EXIT_SUCCESS);
}
