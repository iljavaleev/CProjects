#include "mq_seqnum.h"
#include "errno.h"

#define SEQ_NUM_SIZE 100

static volatile mqd_t smq;
static volatile int dbFd;
static volatile int seqNum = 0;

static void terminate_server()
{
    char buf[SEQ_NUM_SIZE];
    int len = sprintf(buf, "%d\n", seqNum);
    if(write(dbFd, buf, len) == -1)
    {
        if (close(dbFd) == -1)
            printf("error close\n");
        SIGNAL_ERROR("write to db");
    }

    if (close(dbFd) == -1)
        SIGNAL_ERROR("close db");

    if (mq_unlink(SERVER_MQ) == -1)
        SIGNAL_ERROR("close server mq");
   
    _exit(1);
}


static void termination_handler(int signum) 
{
    if (signum == SIGINT || signum == SIGTERM) 
        terminate_server(); 
}


int main(int argc, char *argv[])
{
    int timeout;
    char clientMQ[MQ_NAME_LEN];
    struct request req;
    struct response resp;
    char buf[SEQ_NUM_SIZE];
    
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = termination_handler;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) ERROR("sigaction SIGINT");
    if (sigaction(SIGTERM, &sa, NULL) == -1) ERROR("sigaction SIGTERM");
 
    if (access(SERVER_DB, F_OK) == -1)
    {
        dbFd = open(SERVER_DB, O_WRONLY | O_CREAT | O_SYNC, 0600);
        seqNum = 0;
        printf("start\n");
    }
    else
    {
        dbFd = open(SERVER_DB, O_RDWR);
        int bytes_read;
        while ((bytes_read = read(dbFd, buf, sizeof(buf) - 1)) > 0) 
        {
            buf[bytes_read] = 0; 
            seqNum = atoi(buf);
        }

        if (ftruncate(dbFd, 0) == -1) ERROR("ftruncate failed");
   
        if (lseek(dbFd, 0, SEEK_SET) == -1) ERROR("lseek failed");
    } 
    
    int flags = O_RDWR | O_CREAT | O_EXCL;
    mode_t perms = S_IRUSR | S_IWUSR;
    
    struct mq_attr attr, *attrp;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(struct request);
    attrp = &attr;
    
    mqd_t smq = mq_open(SERVER_MQ, flags, perms, attrp);
    if (smq == (mqd_t) -1)
    {
        ERROR("error create queue");
    }
    

    void *buffer = malloc(attr.mq_msgsize);
    if (buffer == NULL)
        ERROR("malloc");
    
    for(;;)
    {
         
        if (mq_receive(smq, buffer, attr.mq_msgsize, NULL) != sizeof(struct request))
        {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;    
        }
        req = *((struct request*) buffer);
        
   
        snprintf(clientMQ, MQ_NAME_LEN, CLIENT_MQ_TEMPLATE, (long) req.pid);
        mqd_t cmq = mq_open(clientMQ, O_WRONLY);
        if (cmq == (mqd_t) -1)
        {
            fprintf(stderr, "Error open client mq\n");
            continue;

        }
        printf("after\n"); 
        struct timespec timeout;
        if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) 
            ERROR("clock_gettime");

        timeout.tv_sec += DELAY_SECONDS;
        resp.seqNum = seqNum;
        
        if (mq_timedsend(cmq, (void*) &resp, sizeof(struct response), 1, &timeout) == -1)
        {
            fprintf(stderr, "Error writing to MQ %s\n", clientMQ);
        }
    
        if (mq_close(cmq) == -1)
            printf("close\n");
         
        seqNum += req.seqLen; 
    }
    
    
    exit(EXIT_SUCCESS);
}
