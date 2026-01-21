#include <signal.h>
#include <stdlib.h>
#include <signal.h>
#include "fifo_seqnum.h"

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
    int clientFd;
    char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct request req;
    struct response resp;
    char buf[SEQ_NUM_SIZE];
    
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = termination_handler;
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) ERROR("sigaction SIGINT");
    if (sigaction(SIGTERM, &sa, NULL) == -1) ERROR("sigaction SIGTERM");

    struct sigaction sa_for_pipe;
    sigemptyset(&sa_for_pipe.sa_mask);
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa_for_pipe, NULL) == -1) ERROR("sigaction SIGINT");

    if (stop)
        exit(0);

    umask(0); 
    
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
    
    if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 
        && errno != EEXIST) // create FIFO
        ERROR("mkfifo server");
   
    serverFd = open(SERVER_FIFO, O_RDONLY); 
    if (serverFd == -1)
        ERROR("open server fifo");

    int dummyFd = open(SERVER_FIFO, O_WRONLY);
    if (dummyFd == -1)
        ERROR("open dummy end fifo");

   
    for (;;) 
    { 
        if (read(serverFd, &req, sizeof(struct request)) != 
            sizeof(struct request)) 
        {
            fprintf(stderr, "Error reading request; discarding\n");
            continue; 
        }
        
        snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, 
            (long) req.pid);
          
        clientFd = open(clientFifo, O_WRONLY | O_NONBLOCK);

        if (clientFd == -1) 
        { 
            printf("error open %s\n", clientFifo);
            continue;
        }
        
        int flags = fcntl(clientFd, F_GETFL);
        flags &= ~O_NONBLOCK;
        fcntl(clientFd, F_SETFL, flags);

        resp.seqNum = seqNum;
        if (write(clientFd, &resp, sizeof(struct response)) != 
            sizeof(struct response))
            fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
        

        if (close(clientFd) == -1)
            printf("close\n");
        
        seqNum += req.seqLen; 
    }
    
    
    exit(EXIT_SUCCESS);
}
