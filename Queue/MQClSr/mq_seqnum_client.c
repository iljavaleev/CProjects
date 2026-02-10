#include "mq_seqnum.h"
#include <stdlib.h>
#include <unistd.h>

static char clientMQ[MQ_NAME_LEN];


static void removeMQ(void)
{
    if (mq_unlink(clientMQ) == -1)
        ERROR("remove client MQ");
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "—help") == 0)
        ERROR("usage: [seq-len…]\n");
    
    
    snprintf(clientMQ, MQ_NAME_LEN, CLIENT_MQ_TEMPLATE, 
        (long) getpid());
    int flags = O_RDWR | O_CREAT | O_EXCL;
    mode_t perms = S_IRUSR | S_IWUSR;
    
    struct mq_attr attr, *attrp;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct response);
    attrp = &attr; 

    mqd_t cmq = mq_open(clientMQ, flags, perms, attrp);
    if (cmq == (mqd_t) -1)
    {
        ERROR("error create queue");
    }

    if (atexit(removeMQ) != 0)
        ERROR("atexit");
    
    struct request req;
    struct response resp;
    
    req.pid = getpid();
    req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;
    

    mqd_t smq = mq_open(SERVER_MQ, O_WRONLY | O_NONBLOCK);
    if (smq == -1)
        ERROR("open server mq; server is down");
    
    struct timespec timeout;
    if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) 
        ERROR("clock_gettime");

    timeout.tv_sec += DELAY_SECONDS;
    if (mq_timedsend(smq, (char*) &req, sizeof(struct request), 1, &timeout) == -1)
    {
            fprintf(stderr, "Error writing to server MQ");
    }
    
    if (mq_close(smq) == -1)
          printf("close\n");

    
    if (clock_gettime(CLOCK_REALTIME, &timeout) == -1)
        ERROR("clock_gettime"); 
    timeout.tv_sec += DELAY_SECONDS;

    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        ERROR("malloc");
    
    int bytes_read = mq_timedreceive(cmq, buffer, attr.mq_msgsize, NULL, &timeout);
    if (bytes_read == -1)
    {
        ERROR("get response");  
    }
    resp = *((struct response*) buffer);
    printf("%d\n", resp.seqNum);
    
    if (mq_close(cmq) == -1)
        ERROR("close client MQ");

    exit(EXIT_SUCCESS);
}
