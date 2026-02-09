#include "mq_seqnum.h"

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

    if (mq_close(smq) == -1)
        SIGNAL_ERROR("close server mq");
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
    
    int flags = O_RDONLY | O_CREAT | O_EXCL;
    mode_t perms = S_IRUSR | S_IWUSR;
    mqd_t smq = mq_open(SERVER_MQ, flags, perms, NULL);
    if (smq == (mqd_t) -1)
    {
        ERROR("error create queue");
    }
    
    for(;;)
    {
        
    
        if (mq_receive(smq,(char*) &req, sizeof(struct request), NULL) != sizeof(struct request))
        {
            fprintf(stderr, "Error reading request; discarding\n");
            continue;    
        }
    
       
        snprintf(clientMQ, MQ_NAME_LEN, CLIENT_MQ_TEMPLATE, (long) req.pid);
        mqd_t cmq = mq_open(clientMQ, O_WRONLY);
        if (cmq == (mqd_t) -1)
        {
            fprintf(stderr, "Error open client mq\n");
            continue;

        }
        
        struct timespec timeout;
        if (clock_gettime(CLOCK_REALTIME, &timeout) == -1) 
            ERROR("clock_gettime");

        timeout.tv_sec += DELAY_SECONDS;
        resp.seqNum = seqNum;
        
        if (mq_timedsend(cmq, (char*) &resp, sizeof(struct response), 1, &timeout) == -1)
        {
            fprintf(stderr, "Error writing to MQ %s\n", clientMQ);
        }
    
        if (mq_close(cmq) == -1)
            printf("close\n");
        
        seqNum += req.seqLen; 
    }
    
    
    exit(EXIT_SUCCESS);
}
