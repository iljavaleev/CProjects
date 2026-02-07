#include "create.h"

#define SEQ_NUM_SIZE 100

static volatile sig_atomic_t stop = 0;
static volatile int serverFd;
static volatile int dbFd;
static volatile int seqNum;

static void terminate_server()
{
    char buf[SEQ_NUM_SIZE];
    int len = sprintf(buf, "%d\n", seqNum);
    if(write(dbFd, buf, len) == -1)
    {
        if (close(dbFd) == -1)
            printf("error close\n");
        ERROR("write to db");
    }

    if (close(dbFd) == -1)
        ERROR("close db");

    if (close(serverFd) == -1)
        ERROR("close fifo");
    exit(EXIT_SUCCESS);
}


static void termination_handler(int signum) 
{
    if (signum == SIGINT || signum == SIGTERM) 
        terminate_server(); 
}


int main(int argc, char *argv[])
{
    int timeout;
    char clientMQ[100];
    struct request req;
    struct response resp;
    char buf[SEQ_NUM_SIZE];
    
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = termination_handler;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) ERROR("sigaction SIGINT");
    if (sigaction(SIGTERM, &sa, NULL) == -1) ERROR("sigaction SIGTERM");

    if (stop)
        exit(0);

    umask(0); 
    if(create(SERVER_MQ, NULL, NULL) == -1)
    {
        ERROR("server queue error");
    }

    mqd_t smq = mq_open(SERVER_MQ, O_RDONLY);
    if (smq == (mqd_t) -1)
        ERROR("mq_open");
    
    for(;;)
    {
        void *buffer = malloc(attr.mq_msgsize);
        if (buffer == NULL)
            ERROR("malloc");
    
        unsigned int prio;
        if (mq_receive(smq, &req, sizeof(struct request), &prio) != sizeof(struct request))
        {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;    
        }
    
       
        snprintf(clientMQ, 100, CLIENT_MQ_TEMPLATE, (long) req.pid);
        mqd_t cmq = mq_open(clientMQ, O_WRONLY);
        if (cmq == (mqd_t) -1)
            ERROR("mq_open");


        struct timespec timeout;
        if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) 
            ERROR("clock_gettime");

        timeout.tv_sec += delay;
        resp.seqNum = seqNum;
        
        if (mq_timedsend(cmq, (char*) &resp, sizeof(resp), &prio, &delay) == -1)
        {
            fprintf(stderr, "Error writing to MQ %s\n", clientMQ);
        }
    
        
        if (mq_close(cmq) == -1)
            printf("close\n");
        
        seqNum += req.seqLen; 
    }
    
    
    exit(EXIT_SUCCESS);
}
