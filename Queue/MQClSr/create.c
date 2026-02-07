#include "create.h"


static void usageError(const char *progName)
{
    fprintf(stderr, "Usage: %s [-cx] [-m maxmsg] [-s msgsize] mq-name [octal-perms]\n", progName);
    fprintf(stderr, " -c Create queue (O_CREAT)\n");
    fprintf(stderr, " -m maxmsg Set maximum # of messages\n");
    fprintf(stderr, " -s msgsize Set maximum message size\n");
    fprintf(stderr, " -x Create exclusively (O_EXCL)\n");
    exit(EXIT_FAILURE);
}


int create(char *mqname, int maxmsg, int msgsize)
{
    struct mq_attr attr, *attrp;

    attrp = NULL;
    attr.mq_maxmsg = maxmsg ? maxmsg : 10;
    attr.mq_msgsize = msgsize ? masgsize : 2048;
    
    int flags = O_RDWR | O_CREAT | O_EXCL;
    
    mode_t perms = S_IRUSR | S_IWUSR;
    mqd_t mqd = mq_open(mqname, flags, perms, attrp);
    if (mqd == (mqd_t) -1)
    {
        printf("error create queue");
        return -1;
    }

    return 0;
}

