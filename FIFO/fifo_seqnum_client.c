#include "fifo_seqnum.h"
#include <stdlib.h>

static char clientFifo[CLIENT_FIFO_NAME_LEN];


static void removeFifo(void)
{
    unlink(clientFifo);
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "—help") == 0)
        ERROR("usage: [seq-len…]\n");
    
    umask(0);
    
    snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, 
        (long) getpid());
    
    if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && 
        errno != EEXIST)
        ERROR("mkfifo client");
    
    if (atexit(removeFifo) != 0)
        ERROR("atexit");
    
    if (atexit(removeFifo) != 0) 
        ERROR("atexit");
    

    struct request req;
    struct response resp;
    
    req.pid = getpid();
    req.seqLen = (argc > 1) ? atoi(argv[1]) : 1;
    
    int serverFd = open(SERVER_FIFO, O_WRONLY | O_NONBLOCK);
    if (serverFd == -1)
        ERROR("open server fifo; server is down");
    
    int flags = fcntl(serverFd, F_GETFL);
    flags &= ~O_NONBLOCK;
    fcntl(serverFd, F_SETFL, flags);

    if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
        ERROR("Can't write to server");

    int clientFd = open(clientFifo, O_RDONLY);
    if (clientFd == -1)
        ERROR("open client fifo");

    if (read(clientFd, &resp, sizeof(struct response)) != 
        sizeof(struct response))
        ERROR("Can't read response from server");

    printf("%d\n", resp.seqNum);
    
    if (close(clientFd) == -1)
        ERROR("close client fifo");

    exit(EXIT_SUCCESS);
}
