#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>


#define ERROR(str){ printf("%s\n", str); exit(1); }


int main(int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
        ERROR("prog mq-name\n");
    
    mqd_t mqd = mq_open(argv[1], O_RDONLY);
    if (mqd == (mqd_t) -1)
        ERROR("mq_open");

    struct mq_attr attr;
    if (mq_getattr(mqd, &attr) == -1)
        ERROR("mq_getattr");


    printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
    printf("Maximum message size: %ld\n", attr.mq_msgsize);
    printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);
    
    exit(EXIT_SUCCESS);
}
