
#include <signal.h>
#include <stdlib.h>
#include "headers.h"

#define SEQ_NUM_SIZE 100

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

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        ERROR("remove");

    exit(EXIT_SUCCESS);
}


static void termination_handler(int signum) 
{
    if (signum == SIGINT || signum == SIGTERM) 
        terminate_server(); 
}


int main(int argc, char *argv[])
{
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
    

    int sfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1)
        ERROR("socket");

    struct sockaddr_un addr;
    if (strlen(SV_SOCK_PATH) > sizeof(addr.sun_path) - 1)
        ERROR("Server socket path too long");

    if (remove(SV_SOCK_PATH) == -1 && errno != ENOENT)
        ERROR("remove");
    
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SV_SOCK_PATH, sizeof(addr.sun_path));

    if (bind(sfd, (struct sockaddr *) &addr,  sizeof(struct sockaddr_un)) == -1)
        ERROR("bind");

    if (listen(sfd, BACKLOG) == -1)
        ERROR("listen");
    
    int cfd;
    ssize_t numRead;
    for (;;) 
    { 
        cfd = accept(sfd, NULL, NULL);
        if (cfd == -1)
            ERROR("accept");
        
        if (read(cfd, &req, sizeof(struct request)) != 
            sizeof(struct request)) 
        {
            fprintf(stderr, "Error reading request; discarding\n");
            continue; 
        }
        
        resp.seqNum = seqNum;
        if (write(cfd, (void*)&resp, sizeof(struct response)) != 
            sizeof(struct response))
            fprintf(stderr, "Error writing to socket %d\n", cfd);
        
        if (close(cfd) == -1)
            ERROR("close");

        seqNum += req.seqLen; 
    }
    
    exit(EXIT_SUCCESS);
}
